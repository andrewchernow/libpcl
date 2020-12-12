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

#include "_error.h"
#include <pcl/buf.h>
#include <pcl/io.h>
#include <pcl/alloc.h>

static char *
escape_jsonstr(pcl_buf_t *b, const char *s)
{
	pcl_buf_reset(b);

	for(; *s; s++)
	{
		switch(*s)
		{
			case '\\':
				pcl_buf_putstr(b, "\\\\");
				break;

			case '/':
				pcl_buf_putstr(b, "\\/");
				break;

			case '"':
				pcl_buf_putstr(b, "\\\"");
				break;

			case '\b':
				pcl_buf_putstr(b, "\\b");
				break;

			case '\t':
				pcl_buf_putstr(b, "\\t");
				break;

			case '\n':
				pcl_buf_putstr(b, "\\n");
				break;

			case '\f':
				pcl_buf_putstr(b, "\\f");
				break;

			case '\r':
				pcl_buf_putstr(b, "\\r");
				break;

			default:
			{
				/* control characters */
				if(*s < 0x20)
					pcl_buf_putf(b, "\\u%04x", (int) *s);
				else
					pcl_buf_putchar(b, *s);
			}
		}
	}

	return b->data;
}

char *
pcl_err_vjson(const char *message, va_list ap)
{
	pcl_err_t *err = pcl_err_get();
	pcl_buf_t *b = err->buffer;
	pcl_buf_t escbuf;

	pcl_buf_init(&escbuf, 256, PclBufText);

	if(!b)
		b = err->buffer = pcl_buf_init(NULL, 512, PclBufText);
	else
		pcl_buf_reset(b);

	char *msg = NULL;

	if(!strempty(message))
		pcl_vasprintf(&msg, message, ap);

	pcl_buf_putf(b,"{\"err\":%d,\"oserr\":%u,\"msg\":", err->err, err->oserr);

	if(strempty(msg))
		pcl_buf_putstr(b, "null");
	else
		pcl_buf_putf(b, "\"%s\"", escape_jsonstr(&escbuf, msg));

	pcl_free_safe(msg);

	pcl_buf_putstr(b, ",\"strace\":[");

	for(pcl_err_trace_t *t = err->strace; t; t = t->next)
	{
		pcl_buf_putstr(b, "{\"file\":");

		if(strempty(t->file))
			pcl_buf_putstr(b, "null");
		else
			pcl_buf_putf(b, "\"%s\"", escape_jsonstr(&escbuf, t->file));

		pcl_buf_putstr(b, ",\"func\":");

		if(strempty(t->func))
			pcl_buf_putstr(b, "null");
		else
			pcl_buf_putf(b, "\"%s\"", escape_jsonstr(&escbuf, t->func));

		pcl_buf_putf(b, ",\"line\":%d,\"msg\":", t->line);

		if(strempty(t->msg))
			pcl_buf_putstr(b, "null");
		else
			pcl_buf_putf(b, "\"%s\"", escape_jsonstr(&escbuf, t->msg));

		pcl_buf_putchar(b, '}');

		if(t->next)
			pcl_buf_putchar(b, ',');
	}

	pcl_buf_putstr(b, "]}");

	pcl_buf_clear(&escbuf);

	/* steal data pointer */
	char *json = b->data;
	b->data = NULL;
	pcl_buf_clear(b);

	return json;
}
