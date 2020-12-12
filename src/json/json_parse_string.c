/*
  Portable C Library (PCL)
  Copyright (c) 1999-2003, 2005-2014, 2017-2020 Andrew Chernow
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
#include <pcl/buf.h>

char *
ipcl_json_parse_string(ipcl_json_state_t *s)
{
	if(*s->next != '"')
		JSON_THROW("expected opening quote", 0);

	pcl_buf_t buf;
	pcl_buf_t *b = pcl_buf_init(&buf, 32, PclBufText);

	s->ctx = s->next++;

	for(; *s->next; s->next++)
	{
		if(*s->next != '\\')
		{
			if(*s->next == '"')
				break;

			pcl_buf_putchar(b, *s->next);
			continue;
		}

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
				break;

			default:
				pcl_buf_putf(b, "\\%c", *s->next);
		}
	}

	if(*s->next != '"')
	{
		pcl_buf_clear(b);
		JSON_THROW("expected closing double quote", 0);
	}

	s->next++;

	return b->data;
}
