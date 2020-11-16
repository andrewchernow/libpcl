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

#include <pcl/log.h>
#include <pcl/time.h>
#include <pcl/vector.h>
#include <pcl/errctx.h>
#include <pcl/error.h>
#include <pcl/stdio.h>
#include <pcl/string.h>
#include <pcl/stdlib.h>
#include <stdarg.h>

int
pcl_log_vevent(pcl_log_event_t *e, const char *message, va_list ap)
{
	if(!e)
		return 0;

	char timestr[128];
	char *msg = NULL;
	char *location = NULL;
	pcl_err_ctx_t *ctx = e->ctx ? e->ctx : pcl_err_ctx();

	ctx->frozen = 1;

	/* format msg */
	if(!strempty(message))
		(void) pcl_vasprintf(&msg, message, ap);

	/* format when...time */
	pcl_tm_t when;

	if(e->level & PCL_LOGOPT_UTC)
		pcl_gmtime(&e->when, &when);
	else
		pcl_localtime(&e->when, &when);

	(void) pcl_strftime(timestr, countof(timestr), PCL_LOG_TIMEFORMAT, &when);

	char file[512] = {0};
	char func[200] = {0};
	if(!strempty(e->file))
	{
		pcl_strcpy(file, sizeof(file), e->file);
#ifdef PCL_WINDOWS
		pcl_strrepchr(file, '\\', '/');
#endif
	}

	if(!strempty(e->func))
	{
		pcl_strcpy(func, sizeof(func), e->func);
#ifdef PCL_WINDOWS
		pcl_strrepchr(func, '\\', '/');
#endif
	}

	/* format location */
	if(!strempty(file) || !strempty(func) || e->line > 0)
	{
		char linestr[16] = {0};

		if(e->line > 0)
			(void) pcl_sprintf(linestr, countof(linestr), "(%d)", e->line);

		(void) pcl_asprintf(&location, "%s%s%s%s: ",
			*file ? file : "",
			*file && *func ? ":" : "",
			*func ? func : "",
			linestr);
	}

	/* Example output: (see pcl_err_fprintf) - first line is log specific
	 *
	 * [Jun 11 05:27:12 facility.level] file:func(line): There was a problem with something
	 *   some.c:func(463): optional trace message
	 *     another.c:getuserid(2029)
	 *       query.c:select(99): PCL_EBUF - buffer too small
	 *         >An optional user message goes here
	 */

	int count = pcl_fprintf(e->stream, "[%s %s%s%s] %s%s\n",
		timestr,
		strempty(e->facility) ? "" : e->facility,
		strempty(e->facility) ? "" : ".",
		pcl_log_strlevel(e->level),
		location ? location : "",
		msg ? msg : "");

	if(PCL_LOGLEV_HASTRACE(e->level))
		count += pcl_err_ctx_fprintf(ctx, e->stream, 2, NULL);

	fflush(e->stream);
	pcl_free_s(location);
	pcl_free_s(msg);
	ctx->frozen = 0;

	/* Exit Policy: should this level cause an app `exit(3)` */
	if(e->exit_levels & e->level)
	{
		/* callee provided an exit_handler */
		if(e->exit_handler)
		{
			e->ctx = ctx;
			e->exit_handler(e, e->exit_context);
		}

		/* exit_handler was NULL or provided one didn't exit. No need to free ctx. */
		exit(e->exit_code);
	}

	return count;
}


