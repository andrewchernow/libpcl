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

#include "_time.h"
#include <pcl/error.h>
#include <string.h>

#define COND_INTERVAL 10000000 // nanoseconds (10 milliseconds)

#ifdef PCL_WINDOWS
static int
win32_nanosleep(uint64_t ns)
{
	HANDLE timer;
	LARGE_INTEGER li;

	if(!(timer = CreateWaitableTimer(NULL, TRUE, NULL)))
		return SETLASTERR();

	/* windows uses 100ns intervals, value must be negative -- see docs */
	li.QuadPart = - (LONGLONG) (ns / 100);

	if(!SetWaitableTimer(timer, &li, 0, NULL, NULL, FALSE))
	{
		SETLASTERR();
		CloseHandle(timer);
		return -1;
	}

	DWORD r = WaitForSingleObject(timer, INFINITE);
	CloseHandle(timer);

	return r == WAIT_OBJECT_0 ? 0 : SETLASTERR();
}
#endif

static bool
sleep_cond(uint64_t nsecs, bool (*cond)(void), uint32_t cond_interval)
{
	if(cond())
		return true;

	if(cond_interval == 0)
		cond_interval = COND_INTERVAL;

	do
	{
		uint64_t s = min(nsecs, cond_interval);

		/* recurse but with NULL cond callback */
		if(pcl_sleep(s, NULL, 0))
			return R_TRC(false);

		nsecs -= s;

		if(cond())
			break;
	}
	while(nsecs > 0);

	return true;
}

bool
pcl_sleep(uint64_t nsecs, bool (*cond)(void), uint32_t cond_interval)
{
	if(cond)
		return sleep_cond(nsecs, cond, cond_interval);

#ifdef PCL_WINDOWS
	if(win32_nanosleep(nsecs))
		return R_TRC(false);

#else
	struct timespec amt, remaining_amt;

	amt.tv_sec = (time_t) (nsecs / PCL_NSECS);
	amt.tv_nsec = (long) (nsecs % PCL_NSECS);

	while(nanosleep(&amt, &remaining_amt))
	{
		/* failed (very unlikely) */
		if(errno != EINTR)
			return R_SETLASTERR(false);

		/* EINTR, continue sleeping for the remaining amount */
		memcpy(&amt, &remaining_amt, sizeof(amt));
	}
#endif

	return true;
}
