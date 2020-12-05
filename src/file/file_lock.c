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

int
pcl_file_lock(pcl_file_t *file, int operation)
{
	if(!file || file->fd == PCL_BADFD)
		return BADARG();

	switch(operation)
	{
		case PCL_RDLOCK:
		case PCL_WRLOCK:
		case PCL_UNLOCK:
			break;

		default:
			return SETERRMSG(PCL_EINVAL, "Unknown lock operation: %d", operation);
	}

#ifdef PCL_UNIX
	struct flock fl = {
		.l_whence = SEEK_SET,
		.l_start = 0,
		.l_len = 0,
		.l_type = (short) operation,
		.l_pid = getpid()
	};

	int type = file->flags & PCL_FF_NONBLOCK ? F_SETLK : F_SETLKW;

	if(fcntl(file->fd, type, &fl) == -1)
		return SETLASTERR();

	if(fl.l_type == PCL_UNLOCK)
		file->flags &= ~PCL_FF_FLOCK;
	else
		file->flags |= PCL_FF_FLOCK;
#else
	OVERLAPPED ol = {0};
	DWORD flags = 0;

	/* unlock */
	if(operation == PCL_UNLOCK)
	{
		if(!UnlockFileEx(file->fd, 0, MAXDWORD, MAXDWORD, &ol))
			return SETLASTERR();

		file->flags &= ~PCL_FF_FLOCK;
		return 0;
	}

	if(operation == PCL_WRLOCK)
		flags |= LOCKFILE_EXCLUSIVE_LOCK;

	if(file->flags & PCL_FF_NONBLOCK)
		flags |= LOCKFILE_FAIL_IMMEDIATELY;

	if(!LockFileEx(file->fd, flags, 0, MAXDWORD, MAXDWORD, &ol))
	{
		if((file->flags & PCL_FF_NONBLOCK) && pcl_oserrno == ERROR_IO_PENDING)
			return SETERR(PCL_EAGAIN);

		return SETLASTERR();
	}

	file->flags |= PCL_FF_FLOCK;
#endif

	return 0;
}
