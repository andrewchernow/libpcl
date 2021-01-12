/*
  Portable C Library ("PCL")
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

#include <pcl/log.h>
#include <pcl/time.h>
#include <stdlib.h>
#include <stdio.h>

static void
default_exit_handler(pcl_log_event_t *e, void *context)
{
	UNUSED(context);

	fprintf(e->stream, "***PANIC at %s:%s(%d)***\n",
		e->file ? e->file : "",
		e->func ? e->func : "",
		e->line);

	exit(e->exit_code);
}

/* placeholder function used by PCL_LOG_EVENT macro. */
int
pcl_log_event_default(PCL_LOCATION_PARAMS, uint32_t level, const char *message, ...)
{
	/* enable tracing except for INFO and WARN events */
	if(level != PCL_LOGLEV_INFO && level != PCL_LOGLEV_WARN)
		level |= PCL_LOGLEV_TOTRACE(level);

	/* The NULL value members can be omitted from declaration, since C99 states
	 * if omitted they are initialized to zero. However, this function serves
	 * as an example implementation for PCL_LOG_EVENT macro and its best to
	 * have all pcl_log_event_t members accounted for.
	 */
	pcl_log_event_t e = {
		/* streams */
		.stream        = stderr,

		/* location */
		.file          = file,
		.func          = func,
		.line          = line,

		/* event details */
		.when          = pcl_time(),
		.level         = level,
		.facility      = "libpcl",

		/* exit config: like panic or even fatal */
		.exit_code     = EXIT_FAILURE,
		.exit_levels   = PCL_LOGLEV_PANIC,
		.exit_handler  = default_exit_handler,
		.exit_context  = NULL
	};

	/* proxy to pcl_log_vevent */
	va_list ap;
	va_start(ap, message);
	int w = pcl_log_vevent(&e, message, ap);
	va_end(ap);

	return w;
}
