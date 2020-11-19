/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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

#include "_errctx.h"
#include <pcl/buf.h>
#include <pcl/stdio.h>

pcl_buf_t *
ipcl_err_ctx_serialize(pcl_err_ctx_t *ctx, int indent, const char *format, va_list ap)
{
	pcl_err_trace_t *t = ctx->strace;

	if(!t || ctx->err == PCL_EOKAY)
		return NULL;

	char line[24];
	bool hasfile, hasfunc, hasmsg;
	pcl_buf_t *b = ctx->buffer;

	if(!b)
		b = ctx->buffer = pcl_buf_init(NULL, 512, true);
	else
		pcl_buf_reset(b); // resue it (again, per-thread)

	if(!strempty(format))
	{
		int len;
		char *data;

		pcl_buf_putf(b, "%*s", indent, indent ? " " : "");
		pcl_buf_vputf(b, format, ap);

		/* ensure message is terminated with LF */
		data = pcl_buf_data(b, &len);
		if(data[len - 1] != '\n')
			pcl_buf_putchar(b, '\n');

		indent += 2;
	}

	for(; t; t = t->next, indent += 2)
	{
		/* don't handle tail here. It's formatted differently being the actual error site */
		if(!t->next)
			break;

		hasmsg = !strempty(t->msg);
		hasfile = !strempty(t->file);
		hasfunc = !strempty(t->func);

		*line = 0;
		if(t->line > 0)
			pcl_sprintf(line, sizeof(line), "(%d)", t->line);

		pcl_buf_putf(b, "%*s%s%s%s%s%s%s\n",
			indent, indent ? " " : "",
			hasfile ? t->file : "",
			hasfile && hasfunc ? ":" : "", /* if we have both, include separator */
			hasfunc ? t->func : "",
			line,
			hasmsg ? ": " : "",
			hasmsg ? t->msg : "");
	}

	/* 't' is stack trace tail, where the actual error occured */
	hasfile = !strempty(t->file);
	hasfunc = !strempty(t->func);

	*line = 0;
	if(t->line > 0)
		pcl_sprintf(line, sizeof(line), "(%d)", t->line);

	pcl_buf_putf(b, "%*s%s%s%s%s%s%s - %s\n",
		indent, indent ? " " : "",
		hasfile ? t->file : "",
		hasfile && hasfunc ? ":" : "", /* if we have both, include separator */
		hasfunc ? t->func : "",
		line,
		(hasfile || hasfunc || *line) ? ": " : "",
		pcl_err_name(ctx->err),
		pcl_err_msg(ctx->err));

	/* print actual error message, this could be empty if a context msg was not provided. */
	if(!strempty(t->msg))
		pcl_buf_putf(b, "%*s>%s\n", indent + 2, " ", t->msg);

	return b;
}
