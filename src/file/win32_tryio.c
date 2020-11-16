/*
  Portable C Library (PCL)
  Copyright (c) 1999-2003, 2005-2014, 2017-2020 Andrew Chernow
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
#include <pcl/time.h>
#include <pcl/thread.h>
#include <pcl/error.h>

static int
tryio(pcl_file_t *file, struct ipcl_win32_aio *aio, void *buf,
	size_t count, int ms_timeout, bool isread)
{
	bool okay;
	pcl_clock_t start_time = pcl_clock();

	const char *errmsg = NULL;

	if(isread && !(file->flags & PCL_FF_READ))
		errmsg = "file not opened for read operations";
	else if(!isread && !(file->flags & PCL_FF_WRITE))
		errmsg = "file not opened for write operations";

	if(errmsg)
		return SETERRMSG(PCL_ENOTSUP, errmsg, 0);

	if(!(file->flags & PCL_FF_NONBLOCK))
		return SETERRMSG(PCL_ENOTSUP, "file not opened for async I/O", 0);

	DWORD oserr = 0;
	uint64_t me = pcl_thread_id();
	OVERLAPPED *ol = &aio->ol;

	if(aio->pending)
	{
		/* can't start IO op in one thread and resume it in another */
		if(aio->pending != me)
			return SETERRMSG(PCL_EACCES, "cannot resume async I/O that was started "
																	 "by another thread '%u'", aio->pending);

		/* async op still pending, needs to be completed or cancelled. Do not
		 * issue another one via ReadFile or WriteFile.
		 */
		goto PENDING_IO;
	}
	else
	{
		aio->pending = me; /* new async request */
	}

	/* MSDN: "This member is nonzero only when performing I/O requests on a
	 * seeking device that supports the concept of an offset (also referred
	 * to as a file pointer mechanism), such as a file. Otherwise, this
	 * member must be zero."...thus a pipe.
	 */
	if(GetFileType(file->fd) == FILE_TYPE_PIPE)
	{
		ol->Offset = 0;
		ol->OffsetHigh = 0;
	}
	else
	{
		LARGE_INTEGER li;

		if((li.QuadPart = pcl_seek(file, 0, SEEK_CUR)) == -1)
		{
			aio->pending = 0; /* error, clear pending since async op never issued */
			return TRC();
		}

		ol->Offset = li.LowPart;
		ol->OffsetHigh = li.HighPart;
	}

	/* issue the async operation */
	SetLastError(0);
	okay = isread ? ReadFile(file->fd, buf, (DWORD) count, NULL, ol)
								: WriteFile(file->fd, (const void *) buf, (DWORD) count, NULL, ol);
	if(okay)
	{
PENDING_IO:

		DWORD amt;

		if(GetOverlappedResult(file->fd, ol, &amt, false))
		{
			aio->pending = 0; /* consumed, nothing pending */
			return (int) amt;  /* success */
		}
	}

	switch((oserr = GetLastError()))
	{
		case ERROR_IO_PENDING:
		case ERROR_IO_INCOMPLETE:
			break; /* time to wait */

			/* something cancelled our I/O request. MSDN: "I/O operations that are
			 * canceled complete with the error ERROR_OPERATION_ABORTED."
			 */
		case ERROR_OPERATION_ABORTED:
			return SETOSERR(oserr);

		case ERROR_HANDLE_EOF:          /* done, must return zero in this case */
		case ERROR_INVALID_USER_BUFFER: /* too many outstanding async requests, see ReadFile docs */
		case ERROR_NOT_ENOUGH_MEMORY:   /* too many outstanding async requests, see ReadFile docs */
		case ERROR_BROKEN_PIPE:         /* write end of pipe closed and we are trying to read */
		default:                        /* anything else handled the same way */
			goto CANCEL_IO;
	}

	/* Always check signaled state of OVERLAPPED.hEvent even if ms_timeout
	 * is 0 or already expired. Thus, don't raise a timeout error below. Just
	 * make sure ms_timeout is properly adjusted for WaitForSingleObject.
	 */
	if(ms_timeout > 0)
	{
		ms_timeout = (int) ((double) ms_timeout - pcl_msecdiff(start_time, pcl_clock()));
		if(ms_timeout < 0)
			ms_timeout = 0;
	}

	/* Too bad we can't use GetOverlappedResultEx(), which has a timeout
	 * argument. That function didn't appear until Windows 8/Server 2012.
	 */
	switch(WaitForSingleObject(ol->hEvent, ms_timeout))
	{
		/* expose as EAGAIN but set OS error to WAIT_TIMEOUT. This way, callee can
		 * always dive deeper by calling `pcl_err_ctx()->oserr`.
		 */
		case WAIT_TIMEOUT:
			return SETERR_OS(PCL_EAGAIN, WAIT_TIMEOUT);

			/* overlapped event object was signaled */
		case WAIT_OBJECT_0:
		{
			DWORD amt;

			SetLastError(0);
			if(GetOverlappedResult(file->fd, ol, &amt, false))
			{
				aio->pending = 0; /* consumed, nothing pending */
				return (int) amt;  /* success */
			}

			oserr = GetLastError();
			if(oserr == ERROR_IO_INCOMPLETE)
				return SETERR_OS(PCL_EAGAIN, oserr); /* not ready yet */

			break; /* cancellable error */
		}

		case WAIT_FAILED:
		default:
			oserr = GetLastError();
			break; /* cancellable error */
	}

CANCEL_IO:;
	aio->pending = 0;
	(void) CancelIoEx(file->fd, ol);
	return (isread && oserr == ERROR_HANDLE_EOF) ? 0 : SETOSERR(oserr);
}

int
pcl_tryread(pcl_file_t *file, void *buf, size_t count, int ms_timeout)
{
	if(!file || !buf)
		return BADARG();

	int r = tryio(file, &file->aio_read, buf, count, ms_timeout, true);
	return r < 0 ? TRC() : r;
}

int
pcl_trywrite(pcl_file_t *file, const void *data, size_t count, int ms_timeout)
{
	if(!file || !data)
		return BADARG();

	int r = tryio(file, &file->aio_write, (void *) data, count, ms_timeout, false);
	return r ? TRC() : 0;
}
