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

#include "../errctx/_errctx.h"
#include <pcl/stdlib.h>
#include <pcl/stdio.h>
#include <pcl/string.h>
#include <stdarg.h>

int
pcl_err_vtrace(PCL_LOCATION_PARAMS, const char *format, va_list ap)
{
	pcl_err_ctx_t *ctx = pcl_err_ctx();

	if(ctx->frozen)
		return 0;

	/* To avoid issues with a trace occuring in an SO or DLL that's been unloaded, we must allocate
	 * file and func. We can't count on file and func const strings being available. We also allocate
	 * the dynaically sized format message. All of these strings are allocated at end of the trace
	 * struct, so we have one allocation rather than four per trace.
	 */

	pcl_err_trace_t *trc;
	size_t trc_len, file_len = 0, func_len = 0, msg_len = 0;

	if(file)
		file_len = strlen(file) + 1;

	if(func)
		func_len = strlen(func) + 1;

	if(!strempty(format))
	{
		va_list ap2;
		va_copy(ap2, ap);
		msg_len = pcl_vsprintf(NULL, 0, format, ap2) + 1; // query for size
		va_end(ap2);
	}

	trc_len = sizeof(pcl_err_trace_t) + file_len + func_len + msg_len;
	trc = pcl_zalloc(trc_len);
	char *tail = (char *) (trc + 1);

	trc->line = line;
	trc->size = trc_len;
	trc->file = file_len ? tail : NULL;
	trc->func = func_len ? tail + file_len : NULL;
	trc->msg = msg_len ? tail + file_len + func_len : NULL;

	if(trc->file)
	{
		memcpy(trc->file, file, file_len);
#ifdef PCL_WINDOWS
		pcl_strrepchr(trc->file, '\\', '/');
#endif
	}

	if(trc->func)
	{
		memcpy(trc->func, func, func_len);
#ifdef PCL_WINDOWS
		pcl_strrepchr(trc->func, '\\', '/');
#endif
	}

	if(trc->msg)
		pcl_vsprintf(trc->msg, msg_len, format, ap);

	/* add to stack (push) */
	trc->next = ctx->strace;
	ctx->strace = trc;

	return ctx->err == PCL_EOKAY ? 0 : -1;
}
