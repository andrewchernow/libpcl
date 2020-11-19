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
#include <pcl/error.h>

size_t
pcl_strftime(char *out, size_t len, const char *format, const pcl_tm_t *tm)
{
	char buf[4096];

	if(!format || !tm)
		return BADARG();

	/* query for size */
	if(!out || len == 0)
	{
		out = buf;
		len = sizeof(len);
	}

	size_t fmtlen = strlen(format);

	if(len < fmtlen)
		return SETERR(PCL_EBUF);

	struct tm systm = {
		.tm_sec = tm->tm_sec,
		.tm_min = tm->tm_min,
		.tm_hour = tm->tm_hour,
		.tm_mday = tm->tm_mday,
		.tm_mon = tm->tm_mon,
		.tm_year = tm->tm_year,
		.tm_wday = tm->tm_wday,
		.tm_yday = tm->tm_yday,
		.tm_isdst = tm->tm_isdst,
#ifndef PCL_WINDOWS
		.tm_gmtoff = tm->tm_gmtoff,
		.tm_zone = tm->tm_zone
#endif
	};

	return strftime(out, len, format, &systm);
}
