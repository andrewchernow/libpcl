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

#include "test.h"
#include <pcl/time.h>
#include <pcl/stat.h>
#include <string.h>

/* Used in many test cases.
 * "2021-01-06 14:58:29.379774446 UTC"
 */
static pcl_time_t now = {.sec = 1609945109, .nsec = 379774446};

/**$ Compare PCL time with OS time */
TESTCASE(time)
{
	time_t os;
	pcl_time_t pcl = pcl_time();

	time(&os);

	/* times can be slightly off eventhough called back-to-back. Allow a second off. */
	long long diff = (long long) os - pcl.sec;
	ASSERT_TRUE(diff > -2 && diff < 2, "os and pcl time differ");

	return true;
}

/**$ Ensure gmtime works properly */
TESTCASE(gmtime)
{
	pcl_tm_t tm;

	pcl_gmtime(&now, &tm);

	bool match = tm.tm_year == 121 && tm.tm_mon == 0 && tm.tm_mday == 6 && tm.tm_hour == 14 &&
		tm.tm_min == 58 && tm.tm_sec == 29 && tm.tm_nsec == 379774446 && tm.tm_gmtoff == 0;

	ASSERT_TRUE(match, "gmtime did not produce the correct broken down time");
	return true;
}

/**$ perform a UTC (gm) mktime */
TESTCASE(mktime_utc)
{
	pcl_tm_t tm;

	pcl_gmtime(&now, &tm);
	pcl_time_t t = pcl_mktime(&tm, true);
	ASSERT_TRUE(now.sec == t.sec, "seconds don't match after mktime");
	ASSERT_TRUE(now.nsec == t.nsec, "nanoseconds don't match after mktime");

	return true;
}

/**$ perform a local mktime */
TESTCASE(mktime_local)
{
	pcl_tm_t tm;

	pcl_localtime(&now, &tm);
	pcl_time_t t = pcl_mktime(&tm, false);
	ASSERT_TRUE(now.sec == t.sec, "seconds don't match after mktime");
	ASSERT_TRUE(now.nsec == t.nsec, "nanoseconds don't match after mktime");

	return true;
}

/**$ Set file times */
TESTCASE(utimes)
{
	ASSERT_INTEQ(pcl_utimes(_P("test-data.json"), &now, &now, &now), 0, "utimes failed");

	pcl_stat_t st;
	ASSERT_INTEQ(pcl_stat(_P("test-data.json"), &st), 0, "failed to stat file");

	ASSERT_TRUE(memcmp(&st.atime, &now, sizeof(now)) == 0, "atime doesn't match");
	ASSERT_TRUE(memcmp(&st.mtime, &now, sizeof(now)) == 0, "mtime doesn't match");

	/* birthtime (btime) on linux is immutable. */
#ifndef PCL_LINUX
	ASSERT_TRUE(memcmp(&st.btime, &now, sizeof(now)) == 0, "btime doesn't match");
#endif

	return true;
}
