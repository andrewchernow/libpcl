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

#ifndef LIBPCL_TIME_H
#define LIBPCL_TIME_H

#include <pcl/types.h>
#include <time.h>

#ifdef PCL_WINDOWS
	#include <sys/timeb.h>
	#undef tzset
	#define tzset _tzset
#else
	#include <sys/time.h>
#endif

#define PCL_USECS 1000000
#define PCL_NSECS 1000000000
#define PCL_CLOCK_SEC  PCL_NSECS
#define PCL_CLOCK_MSEC PCL_USECS
#define PCL_CLOCK_USEC 1000

#ifdef __cplusplus
extern "C" {
#endif

/* nanosecond time_t */
struct tag_pcl_time
{
	long long sec;
	int nsec;
};

typedef struct
{
	int tm_sec;         /* seconds */
	int tm_min;         /* minutes */
	int tm_hour;        /* hours */
	int tm_mday;        /* day of the month */
	int tm_mon;         /* month */
	int tm_year;        /* year */
	int tm_wday;        /* day of the week */
	int tm_yday;        /* day in the year */
	int tm_isdst;       /* daylight saving time */

	/* 3-4 character zone abbrev: Ex. EST. Windows uses long names */
#ifdef PCL_WINDOWS
	char tm_zone[65];
#else
	char *tm_zone;
#endif

	/* seconds east of UTC, EST = -18000. GNU systems have this. For windows, this is internally
	 * calculated using some CRT functions.
	 */
	long tm_gmtoff;

	/* nanoseconds, PUL extension. */
	int tm_nsec;
} pcl_tm_t;

/* Gets the current UTC time in epoch nanoseconds. */
PCL_EXPORT pcl_time_t pcl_time(void);

PCL_EXPORT int pcl_gmtime(const pcl_time_t *t, pcl_tm_t *result);

PCL_EXPORT int pcl_localtime(const pcl_time_t *t, pcl_tm_t *result);

/**
 * @param out output buffer. Set to NULL to query output length
 * @param len size in bytes of output buffer. If this is 0 or out is NULL, the function
 * merely queries for output length.
 * @param format
 * @param tm
 * @return
 */
PCL_EXPORT size_t pcl_strftime(char *out, size_t len, const char *format, const pcl_tm_t *tm);

PCL_EXPORT int pcl_utimes(const pchar_t *path, pcl_time_t *atime, pcl_time_t *ctime,
	pcl_time_t *mtime, pcl_time_t *crtime, pcl_time_t *btime);

/** Converts broken-down time to epoch nanoseconds.
 * @param tu
 * @param utc true to interrupt \a tu as UTC and false for localtime
 * @return
 */
PCL_EXPORT pcl_time_t pcl_mktime(pcl_tm_t *tu, bool utc);

/** Pause execution of the current thread for the given number of nanoseconds.
 *
 * @param nsecs nanoseconds to sleep, guarenteed to sleep for this amount when cond
 * parameter is NOT provided.
 * @param cond When not NULL, this is called every cond_interval seconds and aborts the
 * sleep operation if it returns true.
 * @param cond_interval nanosecond interval between calls to optional "cond" argument. If this
 * is zero, the default interval of 10000000 ns is used (10 ms). Note: cond is called prior to
 * the first sleep. This argument is ignored if cond is NULL.
 * @return true for success and false otherwise. When cond is not NULL, true can mean the
 * cond returned true or the function slept for the entire nsecs.
 */
PCL_EXPORT bool pcl_sleep(uint64_t nsecs, bool (*cond)(void), uint32_t cond_interval);

PCL_EXPORT pcl_clock_t pcl_clock(void);

PCL_INLINE pcl_clock_t
pcl_secdiff(pcl_clock_t start, pcl_clock_t end)
{
	return (end - start) / PCL_CLOCK_SEC;
}

PCL_INLINE pcl_clock_t
pcl_msecdiff(pcl_clock_t start, pcl_clock_t end)
{
	return (end - start) / PCL_CLOCK_MSEC;
}

PCL_INLINE pcl_clock_t
pcl_usecdiff(pcl_clock_t start, pcl_clock_t end)
{
	return (end - start) / PCL_CLOCK_USEC;
}

#ifdef __cplusplus
}
#endif
#endif
