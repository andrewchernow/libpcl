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

#ifndef LIBPCL__TIME_H
#define LIBPCL__TIME_H

#include <pcl/time.h>

#ifdef PCL_WINDOWS
	#define TIME_HAVE_CONTEXT
	#include <windows.h>
	#include <sys/stat.h>
	#define SECS_1601_1970 11644473600LL /* seconds between 1601 and 1970 */
#elif defined(PCL_LINUX)
	#ifdef CLOCK_MONOTONIC_RAW
		#define CLOCK_MONO CLOCK_MONOTONIC_RAW
	#else
		#define CLOCK_MONO CLOCK_MONOTONIC
	#endif
#elif defined(PCL_DARWIN)
	#define TIME_HAVE_CONTEXT
	#include <mach/mach.h>
	#include <mach/mach_time.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifdef TIME_HAVE_CONTEXT
typedef struct
{
	#ifdef PCL_WINDOWS
	double win_freq;
	#else
	mach_timebase_info_data_t mach_tbase; /* numer, demon */
	struct timespec inittime;             /* nanoseconds since 1-Jan-1970 to init() */
	uint64_t initclock;                   /* ticks since boot to init() */
	#endif
} ipcl_time_context_t;

ipcl_time_context_t *ipcl_time_context(void);
#endif

void ipcl_time_handler(uint32_t which, void *data);

#ifdef PCL_WINDOWS
pcl_time_t ipcl_win32_ftime_to_pcl(FILETIME *ft); /* FILETIME to pcl_time_t */
FILETIME ipcl_win32_pcl_to_ftime(pcl_time_t t);   /* pcl_time_t to FILETIME */
#endif

#ifdef __cplusplus
}
#endif

#endif //LIBPCL__TIME_H__
