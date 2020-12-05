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
#include <pcl/alloc.h>
#include <pcl/defs.h>
#include <pcl/errctx.h>
#include <pcl/error.h>

void
pcl_file_close(pcl_file_t *file)
{
	if(!file)
		return;

	// ignore errors during by below cleanup
	uint32_t oserr = pcl_oserrno;
	pcl_err_ctx_t *ctx = pcl_err_ctx();
	ctx->frozen = true;

	if(file->flags & PCL_FF_FLOCK)
		(void) pcl_file_lock(file, PCL_UNLOCK);

#ifdef PCL_WINDOWS
	bool badfd = file->fd == PCL_BADFD;

	if(file->flags & PCL_FF_NONBLOCK)
	{
		if(file->flags & PCL_FF_READ)
		{
			if(file->aio_read.pending && !badfd)
				(void) CancelIoEx(file->fd, &file->aio_read.ol);

			if(file->aio_read.ol.hEvent)
				CloseHandle(file->aio_read.ol.hEvent);
		}

		if(file->flags & PCL_FF_WRITE)
		{
			if(file->aio_write.pending && !badfd)
				(void) CancelIoEx(file->fd, &file->aio_write.ol);

			if(file->aio_write.ol.hEvent)
				CloseHandle(file->aio_write.ol.hEvent);
		}
	}

	if(!badfd)
		CloseHandle(file->fd);

#else
	if(file->fd != PCL_BADFD)
		(void) close(file->fd);
#endif

	pcl_free(file);

	pcl_setoserrno(oserr);
	ctx->frozen = false;
}
