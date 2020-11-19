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
#include <pcl/tchar.h>

#ifdef PCL_DARWIN
#include <sys/attr.h>
#include <unistd.h>
#endif

static void
convert_times(pcl_stat_t *buf, sys_stat_t *os_buf)
{
	/* Linux stat uses a macro to provide backward compatability with the old st_[a|m|c]time naming.
	 * If that macro is defined, we are being compiled to use st_atim vs. st_atimensec.
	 *
	 * Note: Darwin has both but the way we compile it uses the xtimespec struct member.
	 */

	/* convert to nanoseconds */
	buf->atime.sec = os_buf->st_atime;
#ifdef PCL_DARWIN
	buf->atime.nsec = (int)os_buf->st_atimespec.tv_nsec;
#elif defined(st_atime)
	buf->atime.nsec = (int)os_buf->st_atim.tv_nsec;
#else
	buf->atime.nsec = (int)os_buf->st_atimensec;
#endif

	/* convert to nanoseconds */
	buf->mtime.sec = os_buf->st_mtime;
#ifdef PCL_DARWIN
	buf->mtime.nsec = (int)os_buf->st_mtimespec.tv_nsec;
#elif defined(st_mtime)
	buf->mtime.nsec = (int)os_buf->st_mtim.tv_nsec;
#else
	buf->mtime.nsec = (int)os_buf->st_mtimensec;
#endif

	/* convert to nanoseconds */
	buf->ctime.sec = os_buf->st_ctime;
#ifdef PCL_DARWIN
	buf->ctime.nsec = (int)os_buf->st_ctimespec.tv_nsec;
#elif defined(st_ctime)
	buf->ctime.nsec = (int)os_buf->st_ctim.tv_nsec;
#else
	buf->ctime.nsec = (int)os_buf->st_ctimensec;
#endif

#ifdef PCL_DARWIN
	buf->crtime.sec = os_buf->st_birthtimespec.tv_sec;
	buf->crtime.nsec = os_buf->st_birthtimespec.tv_nsec;
#endif
}

int
ipcl_statent(const tchar_t *path, int fd, pcl_stat_t *buf, int flags)
{
	int n;
	sys_stat_t os_buf;

	if(strempty(path) && fd < 0)
		return BADARG();

	if(fd > -1)
		n = sys_fstat(fd, &os_buf);
	else if(flags & PCL_STAT_LINK)
		n = sys_lstat(path, &os_buf);
	else
		n = sys_stat(path, &os_buf);

	if(n)
		return SETLASTERR();

	if(!buf)
		return 0; // exists check only

	buf->size    = (int64_t)os_buf.st_size;
	buf->rdev    = os_buf.st_rdev;
	buf->dev	   = os_buf.st_dev;
	buf->ino	   = os_buf.st_ino;
	buf->uid	   = os_buf.st_uid;
	buf->gid	   = os_buf.st_gid;
	buf->nlink   = os_buf.st_nlink;
	buf->mode    = os_buf.st_mode;
	buf->blksize = os_buf.st_blksize;
	buf->blocks  = os_buf.st_blocks;

	if(path)
	{
		const tchar_t *name = pcl_tcsrchr(path, PCL_TPATHSEPCHAR);

		if(name)
			name++;
		else
			name = path;

		if(*name == '.')
			buf->flags |= UF_HIDDEN;
	}

	convert_times(buf, &os_buf);

	/* grab backup time (btime) and finder info: The below uses fgetattrlist if 'fd' is
	 * valid, otherwise getattrlist using 'path'
	 */
#ifdef PCL_DARWIN
	buf->flags |= os_buf.st_flags;

	struct
	{
		u_int32_t length;
		struct timespec bcktime;
		char finder_info[32];
	} __attribute__ ((__packed__)) info;

	struct attrlist attr;
	unsigned long opts = (flags & PCL_STAT_LINK) ? FSOPT_NOFOLLOW : 0;

	memset(&attr, 0, sizeof(attr));
	attr.bitmapcount = ATTR_BIT_MAP_COUNT;
	attr.commonattr = ATTR_CMN_BKUPTIME | ATTR_CMN_FNDRINFO;

	if((fd >= 0 && !fgetattrlist(fd, &attr, &info, sizeof(info), opts)) ||
		(path && !getattrlist(path, &attr, &info, sizeof(info), opts)))
	{
		buf->btime.sec = info.bcktime.tv_sec;
		buf->btime.nsec = (int) info.bcktime.tv_nsec;
		memcpy(buf->finder_info, info.finder_info, 32);
	}
#endif

	return 0;
}


