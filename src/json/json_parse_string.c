/*
  Portable C Library (PCL)
  Copyright (c) 1999-2021 Andrew Chernow
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "_json.h"
#include <pcl/strint.h>
#include <pcl/string.h>

static ipcl_json_state_t *
hex2num(ipcl_json_state_t *s, uint32_t *codepoint)
{
	char *end, num[5];

	memcpy(num, s->next, 4);
	num[4] = 0;

	if(pcl_strtoi(num, &end, 16, (int *) codepoint) < 0)
		return R_TRCMSG(NULL, "parsing \\u sequence '%s'", num);

	/* not all characters were decoded, bogus \u0000 input */
	if(*end)
		JSON_THROW("invalid unicode character: '%s'", num);

	s->next += 4;
	return s;
}

/** Convert a UTF-16 codepoint (including possible surrogate pairs) to UTF-8. This first converts
 * a \u0000 codepoint. If it is a surrogate, this will parse the second point in the pair.
 * Thus, this function can consume an additional \u0000 if a surrogate pair is detected. After
 * getting a valid unicode codepoint, it is converted to UTF-8 and stored within provided buffer.
 * @param s pointer to a json parser state obejct
 * @param b pointer to a buffer
 * @return pointer to state object or NULL on error
 */
static ipcl_json_state_t *
utf16_to_utf8(ipcl_json_state_t *s, pcl_buf_t *b)
{
	uint32_t pair[2], code;

	if(s->end - s->next < 4)
		JSON_THROW("unexpected end of input: \\u0000 encoding missing digits", 0);

	if(!hex2num(s, &pair[0]))
		return NULL;

	/* UTF-16 surrogate pair */
	if(pair[0] >= 0xd800 && pair[0] <= 0xdbff)
	{
		if(s->end - s->next < 6)
			JSON_THROW("unexpected end of input: \\u0000 encoding missing digits", 0);

		if(*s->next != '\\' || s->next[1] != 'u')
			JSON_THROW("invalid unicode character: missing UTF-16 surrogate '%.12s'", s->next - 6);

		s->next += 2;

		if(!hex2num(s, &pair[1]))
			return NULL;

		/* invalid UTF-16 surrogate */
		if(pair[1] < 0xdc00 || pair[1] > 0xdfff)
			JSON_THROW("invalid unicode character: surrogate '%.6s'", s->next - 6);

		code = 0x10000;
		code += (pair[0] & 0x03ff) << 10;
		code += (pair[1] & 0x03ff);
	}
	else
	{
		code = pair[0];
	}

	int n = pcl_utf8_encode(code, b->data + b->pos);

	if(n == -1)
		return R_TRC(NULL);

	b->pos += n;
	b->len += n;

	return s;
}

char *
ipcl_json_parse_string(ipcl_json_state_t *s)
{
	if(*s->next != '"')
		JSON_THROW("expected opening quote", 0);

	pcl_buf_t buf;
	pcl_buf_t *b = pcl_buf_init(&buf, 32, PclBufText);

	s->ctx = s->next++;

	/* first encode any escapes */
	for(; *s->next; s->next++)
	{
		if(*s->next != '\\')
		{
			if(*s->next == '"')
				break;

			pcl_buf_putchar(b, *s->next);
			continue;
		}

		/* we have an escape sequence */
		switch(*++s->next)
		{
			case '\\':
			case '/':
			case '"':
				pcl_buf_putchar(b, *s->next);
				break;

			case 'n':
				pcl_buf_putchar(b, '\n');
				break;

			case 'r':
				pcl_buf_putchar(b, '\r');
				break;

			case 'b':
				pcl_buf_putchar(b, '\b');
				break;

			case 'f':
				pcl_buf_putchar(b, '\f');
				break;

			case 't':
				pcl_buf_putchar(b, '\t');
				break;

			case 'u':
			{
				s->next++;

				if(!utf16_to_utf8(s, b))
				{
					pcl_buf_clear(b);
					return NULL;
				}

				s->next--;
				break;
			}

			/* json is very specific about what constitutes a valid escape sequence */
			default:
				JSON_THROW("invalid escape sequence within string \\%c", *s->next);
		}
	}

	if(*s->next != '"')
	{
		pcl_buf_clear(b);
		JSON_THROW("expected closing double quote", 0);
	}

	s->next++;

	/* now check that the string is valid utf8, which the above unescaping did not do */
	if(pcl_utf8_check(b->data, b->len) < 0)
		return R_TRC(NULL);

	return b->data;
}
