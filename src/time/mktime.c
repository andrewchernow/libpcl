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

pcl_time_t
pcl_mktime(pcl_tm_t *tu, bool utc)
{
	pcl_time_t t;
	struct tm tm;

	tm.tm_sec = tu->tm_sec;
	tm.tm_min = tu->tm_min;
	tm.tm_hour = tu->tm_hour;
	tm.tm_mday = tu->tm_mday;
	tm.tm_mon = tu->tm_mon;
	tm.tm_year = tu->tm_year;
	tm.tm_wday = tu->tm_wday;
	tm.tm_yday = tu->tm_yday;
	tm.tm_isdst = tu->tm_isdst;

#ifdef PCL_WINDOWS
	t.sec = utc ? _mkgmtime64(&tm) : _mktime64(&tm);
#else
	t.sec = utc ? timegm(&tm) : mktime(&tm);
#endif

	t.nsec = tu->tm_nsec;
	return t;
}
