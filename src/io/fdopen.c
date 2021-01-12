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

#include "../file/_file.h"
#include <pcl/io.h>
#include <pcl/error.h>

#ifdef PCL_WINDOWS
	#include <io.h> // _open_osfhandle
#endif

FILE *
pcl_fdopen(pcl_file_t *file)
{
	FILE *stream;
	const char *mode;

	if(!file)
	{
		SETERR(PCL_EINVAL);
		return NULL;
	}

	if((file->flags & PCL_FF_RDWR) == PCL_FF_RDWR)
		mode = "w+";
	else if(file->flags & PCL_FF_WRITE)
		mode = "w";
	else
		mode = "r";

#ifdef PCL_WINDOWS
	/* convert HANDLE to an 'fd' */
	int fd = _open_osfhandle((intptr_t)file->fd, 0);

	if(fd == -1)
		return R_SETOSERR(NULL, pcl_err_crt2os(errno));

	/* convert an fd to a 'FILE*', stream */
	if(!(stream = _fdopen(fd, mode)))
	{
		HANDLE h = file->fd;

		FREEZE_ERR
		(
			/* CRT _close() will pass our handle to CloseHandle().  Need to
			 * increase its ref count so we can safely cleanup the CRT fd
			 * w/o closing file->fd (which is the file's win32 HANDLE).
			 */
			DuplicateHandle(GetCurrentProcess(), h, GetCurrentProcess(), &file->fd,
				0, false, DUPLICATE_SAME_ACCESS);

			_close(fd);
		);

		return R_SETOSERR(NULL, pcl_err_crt2os(errno));
	}

#else
	if(!(stream = fdopen(file->fd, mode)))
	{
		SETLASTERR();
		return NULL;
	}
#endif

	/* stole the handle, nuke the file */
	file->fd = PCL_BADFD;
	pcl_file_close(file);

	return stream;
}
