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

#include <pcl/stat.h>
#include <pcl/error.h>
#include <pcl/string.h>
#include <unistd.h>
#include <fcntl.h>

#ifdef PCL_HAVE_CONFIG
#	include "config.h"
#endif

#if defined(HAVE_STATX) && defined(PCL_LINUX)
#	include <sys/sysmacros.h> // makedev
#else
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
	buf->atime.nsec = (int) os_buf->st_atimespec.tv_nsec;
#elif defined(st_atime)
	buf->atime.nsec = (int) os_buf->st_atim.tv_nsec;
#else
	buf->atime.nsec = (int) os_buf->st_atimensec;
#endif

	/* convert to nanoseconds */
	buf->mtime.sec = os_buf->st_mtime;
#ifdef PCL_DARWIN
	buf->mtime.nsec = (int) os_buf->st_mtimespec.tv_nsec;
#elif defined(st_mtime)
	buf->mtime.nsec = (int) os_buf->st_mtim.tv_nsec;
#else
	buf->mtime.nsec = (int) os_buf->st_mtimensec;
#endif

	/* convert to nanoseconds */
	buf->ctime.sec = os_buf->st_ctime;
#ifdef PCL_DARWIN
	buf->ctime.nsec = (int) os_buf->st_ctimespec.tv_nsec;
#elif defined(st_ctime)
	buf->ctime.nsec = (int) os_buf->st_ctim.tv_nsec;
#else
	buf->ctime.nsec = (int) os_buf->st_ctimensec;
#endif

#ifdef PCL_DARWIN
	buf->btime.sec = os_buf->st_birthtimespec.tv_sec;
	buf->btime.nsec = (int) os_buf->st_birthtimespec.tv_nsec;
#endif
}
#endif

int
ipcl_statent(const pchar_t *path, int fd, pcl_stat_t *buf, int flags)
{
	int n;

	if(strempty(path) && fd < 0)
		return BADARG();

#ifdef HAVE_STATX  // linux only call
	struct statx osx;

	if(fd > -1)
		n = statx(fd, "", AT_EMPTY_PATH, STATX_ALL, &osx);
	else if(flags & PCL_STAT_LINK)
		n = statx(AT_FDCWD, path, AT_SYMLINK_NOFOLLOW, STATX_ALL, &osx);
	else
		n = statx(AT_FDCWD, path, 0, STATX_ALL, &osx);

	if(n)
		return SETLASTERR();

	if(!buf)
		return 0; // exists check only

	buf->size       = (int64_t) osx.stx_size;
	buf->rdev       = makedev(osx.stx_rdev_major, osx.stx_rdev_minor);
	buf->dev	      = makedev(osx.stx_dev_major, osx.stx_dev_minor);
	buf->ino	      = osx.stx_ino;
	buf->uid	      = osx.stx_uid;
	buf->gid	      = osx.stx_gid;
	buf->nlink      = osx.stx_nlink;
	buf->mode       = osx.stx_mode;
	buf->blksize    = osx.stx_blksize;
	buf->blocks     = osx.stx_blocks;
	buf->atime.sec  = osx.stx_atime.tv_sec;
	buf->atime.nsec = osx.stx_atime.tv_nsec;
	buf->mtime.sec  = osx.stx_mtime.tv_sec;
	buf->mtime.nsec = osx.stx_mtime.tv_nsec;
	buf->ctime.sec  = osx.stx_ctime.tv_sec;
	buf->ctime.nsec = osx.stx_ctime.tv_nsec;
	buf->btime.sec  = osx.stx_btime.tv_sec;
	buf->btime.nsec = osx.stx_btime.tv_nsec;

	uint64_t attrs = osx.stx_attributes & osx.stx_attributes_mask;

	if(attrs & STATX_ATTR_COMPRESSED)
		buf->flags |= UF_COMPRESSED;
	if(attrs & STATX_ATTR_IMMUTABLE)
		buf->flags |= UF_IMMUTABLE;
	if(attrs & STATX_ATTR_APPEND)
		buf->flags |= UF_APPEND;
	if(attrs & STATX_ATTR_NODUMP)
		buf->flags |= UF_NODUMP;
	if(attrs & STATX_ATTR_ENCRYPTED)
		buf->flags |= UF_ENCRYPTED;

#else
	sys_stat_t os;

	if(fd > -1)
		n = sys_fstat(fd, &os);
	else if(flags & PCL_STAT_LINK)
		n = sys_lstat(path, &os);
	else
		n = sys_stat(path, &os);

	if(n)
		return SETLASTERR();

	if(!buf)
		return 0; // exists check only

	buf->size = (int64_t) os.st_size;
	buf->rdev = os.st_rdev;
	buf->dev = os.st_dev;
	buf->ino = os.st_ino;
	buf->uid = os.st_uid;
	buf->gid = os.st_gid;
	buf->nlink = os.st_nlink;
	buf->mode = os.st_mode;
	buf->blksize = os.st_blksize;
	buf->blocks = os.st_blocks;

	convert_times(buf, &os);

#	ifdef PCL_DARWIN
	buf->flags |= os.st_flags;
#	endif
#endif

	buf->dsize = roundup(buf->size, buf->blksize);

	if(path)
	{
		const pchar_t *name = pcl_pcsrchr(path, PCL_PPATHSEPCHAR);

		if(name)
			name++;
		else
			name = path;

		if(*name == '.')
			buf->flags |= UF_HIDDEN;
	}

	return 0;
}


