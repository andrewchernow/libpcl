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

#include "_file.h"
#include <pcl/error.h>
#include <pcl/io.h>

int
ipcl_file_open(pcl_file_t *file, const pchar_t *path, int pcl_oflags, mode_t mode)
{
	if(pcl_oflags & PCL_O_DIRECTORY)
	{
#ifndef O_DIRECTORY
		sys_stat_t st;

		if(stat(path, &st))
			return SETLASTERR();

		if(!S_ISDIR(st.st_mode))
			return SETERR(PCL_ENOTDIR);
#endif
	}

	if(pcl_oflags & PCL_O_NOFOLLOW)
	{
#ifndef O_NOFOLLOW
		sys_stat_t st;

		if(!sys_lstat(path, &st) && S_ISLNK(st.st_mode))
			return SETERRMSG(PCL_ETYPE, "%Ps is a symbolic link", path);
#endif
	}

	if((file->fd = open(path, ipcl_sysoflags(pcl_oflags), mode)) == -1)
		return SETLASTERR();

	/* this should overwrite SHLOCK */
	if(pcl_oflags & PCL_O_EXLOCK)
	{
#ifdef O_EXLOCK
		file->flags |= PCL_FF_OLOCK; /* obtained lock through open() ... "O"LOCK */
#else
		/* need to manually lock the file */
		if(pcl_file_lock(file, PCL_WRLOCK))
			return TRCMSG("file open cannot aquire lock on '%Ps'", path);

		file->flags |= PCL_FF_FLOCK; /* obtained lock through flock */
#endif
	}
	else if(pcl_oflags & PCL_O_SHLOCK)
	{
#ifdef O_SHLOCK
		file->flags |= PCL_FF_OLOCK; /* obtained lock through open() ... "O"LOCK */
#else
		/* need to manually lock the file */
		if(pcl_file_lock(file, PCL_RDLOCK))
			return TRCMSG("file open cannot aquire lock on '%Ps'", path);

		file->flags |= PCL_FF_FLOCK; /* obtained lock through flock */
#endif
	}

	if(pcl_oflags & PCL_O_TEMP)
		(void) pcl_unlink(path);

	return 0;
}
