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

#include "_time.h"
#include <pcl/event.h>
#include <pcl/defs.h>

#ifdef TIME_HAVE_CONTEXT
static ipcl_time_context_t time_context = {0};
#endif

void
ipcl_time_handler(uint32_t which, void *data)
{
	UNUSED(data);

	switch(which)
	{
		case PCL_EVENT_INIT:
		{
			tzset();

#ifdef PCL_DARWIN
			struct timeval micro;

			mach_timebase_info(&time_context.mach_tbase);
			time_context.initclock = mach_absolute_time();

			(void) gettimeofday(&micro, NULL);
			time_context.inittime.tv_sec = micro.tv_sec;
			time_context.inittime.tv_nsec = micro.tv_usec * 1000;

#elif defined(PCL_WINDOWS)
			LARGE_INTEGER li;

			(void)QueryPerformanceFrequency(&li);
			time_context.win_freq = (double)li.QuadPart / PCL_NSECS;
#endif
			break;
		}

		default:
			break;
	}
}

#ifdef TIME_HAVE_CONTEXT
ipcl_time_context_t *
ipcl_time_context(void)
{
	return &time_context;
}
#endif
