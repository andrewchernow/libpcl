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
#include <poll.h>

int
pcl_file_tryread(pcl_file_t *file, void *buf, size_t count, int ms_timeout)
{
	int r;

	if(!file)
		return BADARG();

	if(!(file->flags & PCL_FF_READ))
		return SETERRMSG(PCL_ENOTSUP, "file not opened for read operations", 0);

	if(!(file->flags & PCL_FF_NONBLOCK))
		return SETERRMSG(PCL_ENOTSUP, "file not opened for async I/O", 0);

	if(ms_timeout > 0)
	{
		struct pollfd fds;

		fds.fd = file->fd;
		fds.events = POLLIN;

		if((r = poll(&fds, 1, ms_timeout)) == -1)
			return SETLASTERR();

		if(r == 0) // it's an EAGAIN error but set OS error to ETIMEDOUT
			return SETERR_OS(PCL_EAGAIN, ETIMEDOUT);

		if(fds.revents & POLLNVAL)
			return SETERRMSG(PCL_EBADF, "file descriptor is invalid or not open", 0);

		if(fds.revents & POLLERR)
			return SETERR(PCL_ECONNRESET);
	}

	if((r = pcl_file_read(file, buf, count)) == -1)
		TRC();

	return r;
}
