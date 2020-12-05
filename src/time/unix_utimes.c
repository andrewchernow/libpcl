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

#include <pcl/stat.h>
#include <pcl/error.h>
#include <string.h>

#ifdef PCL_DARWIN
	#include <sys/attr.h>
	#include <unistd.h>
#endif

#ifdef PCL_UNIX
	#include <sys/time.h> // utimes and lutimes
#endif

int
pcl_utimes(const tchar_t *path, pcl_time_t *atime, pcl_time_t *ctime,
	pcl_time_t *mtime, pcl_time_t *crtime, pcl_time_t *btime)
{
	pcl_stat_t st;
	struct timeval tv[2];

	if(pcl_lstat(path, &st))
		return TRC();

	if(!atime)
		atime = &st.atime;

	if(!mtime)
		mtime = &st.mtime;

	if(!crtime)
		crtime = &st.crtime;

	if(!ctime)
		ctime = &st.ctime;

	if(!btime)
		btime = &st.btime;

#ifdef PCL_DARWIN

	struct attrlist attr;
	unsigned long options = 0;
	struct
	{
		struct timespec crtime, mtime, ctime, atime, btime;
	} __attribute__ ((__packed__)) info;

	memset(&attr, 0, sizeof(attr));
	attr.bitmapcount = ATTR_BIT_MAP_COUNT;
	attr.commonattr = ATTR_CMN_CRTIME | ATTR_CMN_MODTIME |
										ATTR_CMN_CHGTIME | ATTR_CMN_ACCTIME | ATTR_CMN_BKUPTIME;

	if(S_ISLNK(st.mode))
		options = FSOPT_NOFOLLOW;

	info.crtime.tv_sec = crtime->sec;
	info.crtime.tv_nsec = crtime->nsec;

	info.mtime.tv_sec = mtime->sec;
	info.mtime.tv_nsec = mtime->nsec;

	info.ctime.tv_sec = ctime->sec;
	info.ctime.tv_nsec = ctime->nsec;

	info.atime.tv_sec = atime->sec;
	info.atime.tv_nsec = atime->nsec;

	info.btime.tv_sec = btime->sec;
	info.btime.tv_nsec = btime->nsec;

	if(!setattrlist(path, &attr, &info, sizeof(info), options))
		return 0; /* it worked */

	/* some volumes don't support setattrlist(). skip to utime version */
	if(errno != ENOTSUP)
		return SETLASTERR();

	/* fallback to incomplete lutimes or utimes */
#endif /* if darwin */

	tv[0].tv_sec = (long) atime->sec;
	tv[0].tv_usec = atime->nsec / 1000;
	tv[1].tv_sec = (long) mtime->sec;
	tv[1].tv_usec = mtime->nsec / 1000;

	/* Handle symlinks */
	if(S_ISLNK(st.mode))
	{
		if(lutimes(path, &tv[0]))
			return SETLASTERR();
	}
	else if(utimes(path, &tv[0]))
	{
		return SETLASTERR();
	}

	return 0;
}
