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
#include <pcl/string.h>

static int
break_down_time(const pcl_time_t *_t, pcl_tm_t *tu, bool utc)
{
	pcl_time_t t;
	struct tm tm;

	if(!tu)
		return SETERRMSG(PCL_EINVAL, "output storage cannot be null", 0);

	/* use now if _t is NULL*/
	t = !_t ? pcl_time() : *_t;

#ifdef PCL_WINDOWS
	errno_t err = utc ? _gmtime64_s(&tm, &t.sec) : _localtime64_s(&tm, &t.sec);
	if(err)
		return SETOSERR(pcl_err_crt2os(err));
#else
	time_t systime = (time_t) t.sec;
	if(utc)
		gmtime_r(&systime, &tm);
	else
		localtime_r(&systime, &tm);
#endif

	tu->tm_sec = tm.tm_sec;
	tu->tm_min = tm.tm_min;
	tu->tm_hour = tm.tm_hour;
	tu->tm_mday = tm.tm_mday;
	tu->tm_mon = tm.tm_mon;
	tu->tm_year = tm.tm_year;
	tu->tm_wday = tm.tm_wday;
	tu->tm_yday = tm.tm_yday;
	tu->tm_isdst = tm.tm_isdst;

#ifdef PCL_WINDOWS
	if(utc)
	{
		tu->tm_gmtoff = 0;
		pcl_strcpy(tu->tm_zone, sizeof(tu->tm_zone), "GMT");
	}
	else /* pcl_localtime */
	{
		int daylight = 0;

		/* if daylight savings time, get offset in minutes */
		if(tu->tm_isdst)
		{
			_get_daylight(&daylight);
			daylight *= 60;
		}

		_get_timezone(&tu->tm_gmtoff);
		tu->tm_gmtoff -= daylight;

		/* Windows uses West of UTC rather than EAST ... translate */
		tu->tm_gmtoff = -tu->tm_gmtoff;

		size_t n;
		*tu->tm_zone = 0;
		_get_tzname(&n, tu->tm_zone, sizeof(tu->tm_zone), tu->tm_isdst);
	}

#else
	tu->tm_gmtoff = 0;

	/* When utc is true, darwin uses UTC and Linux GMT. It should be GMT, UTC is not a timezone */
	tu->tm_zone = (char *) (utc ? "GMT" : tm.tm_zone);
#endif

	tu->tm_nsec = t.nsec;
	return 0;
}

int
pcl_localtime(const pcl_time_t *t, pcl_tm_t *result)
{
	return break_down_time(t, result, false);
}

int
pcl_gmtime(const pcl_time_t *t, pcl_tm_t *result)
{
	return break_down_time(t, result, true);
}

