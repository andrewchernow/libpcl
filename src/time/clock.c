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

#include "_time.h"

/* nanoseconds */
pcl_clock_t
pcl_clock(void)
{
#ifdef PCL_DARWIN
	ipcl_time_context_t *ctx = ipcl_time_context();

	return (pcl_clock_t) mach_absolute_time() * (pcl_clock_t) ctx->mach_tbase.numer /
				 (pcl_clock_t) ctx->mach_tbase.denom;

#elif defined(PCL_LINUX)
	struct timespec ts;
	clock_gettime(CLOCK_MONO, &ts);
	return (pcl_clock_t)(((pcl_clock_t)ts.tv_sec * PCL_NSECS) + (pcl_clock_t)ts.tv_nsec);

#else
	LARGE_INTEGER li;
	ipcl_time_context_t *ctx = ipcl_time_context();

	(void)QueryPerformanceCounter(&li);

	return (pcl_clock_t)li.QuadPart / ctx->win_freq;
#endif
}
