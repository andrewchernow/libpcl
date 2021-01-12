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
#include <pcl/stat.h>
#include <pcl/error.h>
#include <string.h>
#include <fcntl.h>
#include <sys/stat.h>

#ifdef PCL_DARWIN
#include <sys/attr.h>
#include <unistd.h>
#endif

#include <sys/time.h>

int
pcl_utimes(const pchar_t *path, pcl_time_t *atime, pcl_time_t *mtime, pcl_time_t *btime)
{
	pcl_stat_t st;

	if(pcl_lstat(path, &st))
		return TRC();

	if(!atime)
		atime = &st.atime;

	if(!mtime)
		mtime = &st.mtime;

	if(!btime)
		btime = &st.btime;

#ifdef PCL_DARWIN
	struct attrlist attr;
	unsigned long options = 0;
	struct
	{
		struct timespec crtime, mtime, atime; // ordered by bit value ATTR_CMN_xxx
	} __attribute__ ((__packed__)) info;

	memset(&attr, 0, sizeof(attr));
	attr.bitmapcount = ATTR_BIT_MAP_COUNT;
	attr.commonattr = ATTR_CMN_CRTIME | ATTR_CMN_MODTIME | ATTR_CMN_ACCTIME;

	if(S_ISLNK(st.mode))
		options = FSOPT_NOFOLLOW;

	info.crtime.tv_sec = btime->sec;
	info.crtime.tv_nsec = btime->nsec;

	info.mtime.tv_sec = mtime->sec;
	info.mtime.tv_nsec = mtime->nsec;

	info.atime.tv_sec = atime->sec;
	info.atime.tv_nsec = atime->nsec;

	if(!setattrlist(path, &attr, &info, sizeof(info), options))
		return 0; /* all done, rpeferred method worked */

	/* some volumes don't support setattrlist(). skip to utime version in that case */
	if(errno != ENOTSUP)
		return SETLASTERR();

	/* fallback to incomplete utimensat, lutimes or utimes */
#endif /* if darwin */

	/* if system has utimensat, use that over utimes or lutimes */
#ifdef HAVE_UTIMENSAT
	struct timespec ts[2];

	ts[0].tv_sec = (long) atime->sec;
	ts[0].tv_nsec = atime->nsec;
	ts[1].tv_sec = (long) mtime->sec;
	ts[1].tv_nsec = mtime->nsec;

	/* if this fails, continue on and try utimes */
	if(utimensat(AT_FDCWD, path, ts, AT_SYMLINK_NOFOLLOW) == 0)
		return 0;
#endif

	struct timeval tv[2];

	tv[0].tv_sec = (long) atime->sec;
	tv[0].tv_usec = atime->nsec / 1000;
	tv[1].tv_sec = (long) mtime->sec;
	tv[1].tv_usec = mtime->nsec / 1000;

	/* Handle symlinks */
	if(S_ISLNK(st.mode))
	{
		if(lutimes(path, tv))
			return SETLASTERR();
	}
	else if(utimes(path, tv))
	{
		return SETLASTERR();
	}

	return 0;
}
