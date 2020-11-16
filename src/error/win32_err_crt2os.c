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

#include "_error.h"

#include <windows.h>
#include <RasError.h>

#ifndef ERROR_INTERRUPT
#	define ERROR_INTERRUPT 95
#endif

/* map windows CRT error codes to Win32 error codes */
unsigned long
pcl_err_crt2os(int err)
{
		/* A handful have no good mapping, they are commented out */
		static struct {
			int err;
			DWORD winerr;
		} errmap[] = {
			{0,              ERROR_SUCCESS},
			{EPERM,          ERROR_ACCESS_DENIED},
			{ENOENT,				 ERROR_FILE_NOT_FOUND},
			{EINTR,					 ERROR_INTERRUPT},
			{EIO,						 ERROR_IO_DEVICE},
			{E2BIG,					 ERROR_BAD_ENVIRONMENT},
			{ENOEXEC,				 ERROR_EXE_MARKED_INVALID},
			{EBADF,					 ERROR_INVALID_HANDLE},
			{ECHILD,				 ERROR_WAIT_NO_CHILDREN},
			{EAGAIN,				 ERROR_IO_PENDING},
			{ENOMEM,				 ERROR_NOT_ENOUGH_MEMORY},
			{EACCES,				 ERROR_ACCESS_DENIED},
			{EFAULT,				 ERROR_INVALID_ADDRESS},
			{EBUSY,					 ERROR_BUSY},
			{EEXIST,				 ERROR_ALREADY_EXISTS},
			{EXDEV,					 ERROR_NOT_SAME_DEVICE},
			{ENODEV,				 ERROR_DEVICE_DOES_NOT_EXIST},
			{ENOTDIR,				 ERROR_FILE_NOT_FOUND},
			{EINVAL,				 ERROR_INVALID_PARAMETER},
			{ENFILE,				 ERROR_TOO_MANY_OPEN_FILES},
			{EMFILE,				 ERROR_TOO_MANY_OPEN_FILES},
			{EFBIG,					 ERROR_FILE_TOO_LARGE},
			{ENOSPC,				 ERROR_DISK_FULL},
			{ESPIPE,				 ERROR_SEEK},
			{EROFS,					 ERROR_WRITE_PROTECT},
			{EMLINK,				 ERROR_TOO_MANY_LINKS},
			{EPIPE,					 ERROR_BROKEN_PIPE},
			{ERANGE,				 ERROR_RANGE_NOT_FOUND},
			{ENAMETOOLONG,	 ERROR_FILENAME_EXCED_RANGE},
			{ENOLCK,				 ERROR_LOCK_FAILED},
			{ENOTEMPTY,			 ERROR_DIR_NOT_EMPTY},
			{EILSEQ,				 ERROR_NO_UNICODE_TRANSLATION},
			{STRUNCATE,			 ERROR_BUFFER_TOO_SMALL},
			{ESRCH,          ERROR_PROCESS_ABORTED},
			{ENXIO,          ERROR_SYSTEM_DEVICE_NOT_FOUND},
			//{EISDIR,
			{ENOTTY,         ERROR_IO_DEVICE},
			//{EDOM,
			{EDEADLK,        ERROR_POSSIBLE_DEADLOCK},
			{ENOSYS,         ERROR_NOT_SUPPORTED},
			{EDEADLOCK,      ERROR_POSSIBLE_DEADLOCK},
			{EADDRINUSE,     WSAEADDRINUSE},
			{EADDRNOTAVAIL,  WSAEADDRNOTAVAIL},
			{EAFNOSUPPORT,   WSAEAFNOSUPPORT},
			{EALREADY,       WSAEALREADY},
			{EBADMSG,        WSAEBADF},
			{ECANCELED,      WSAECANCELLED},
			{ECONNABORTED,   WSAECONNABORTED},
			{ECONNREFUSED,   WSAECONNREFUSED},
			{ECONNRESET,     WSAECONNRESET},
			{EDESTADDRREQ,   WSAEDESTADDRREQ},
			{EHOSTUNREACH,   WSAEHOSTUNREACH},
			//{EIDRM,
			{EINPROGRESS,    WSAEINPROGRESS},
			{EISCONN,        WSAEISCONN},
			{ELOOP,          WSAELOOP},
			{EMSGSIZE,       WSAEMSGSIZE},
			{ENETDOWN,       WSAENETDOWN},
			{ENETRESET,      WSAENETRESET},
			{ENETUNREACH,    WSAENETUNREACH},
			{ENOBUFS,        WSAENOBUFS},
			{ENODATA,        WSANO_DATA},
			//{ENOLINK,
			//{ENOMSG,
			{ENOPROTOOPT,    WSAENOPROTOOPT},
			{ENOSR,          ERROR_NO_SYSTEM_RESOURCES},
			//{ENOSTR,
			{ENOTCONN,       WSAENOTCONN},
			{ENOTRECOVERABLE,WSANO_RECOVERY},
			{ENOTSOCK,       WSAENOTSOCK},
			{ENOTSUP,        ERROR_NOT_SUPPORTED},
			{EOPNOTSUPP,     WSAEOPNOTSUPP},
			//{EOTHER,       huh?
			{EOVERFLOW,      ERROR_ARITHMETIC_OVERFLOW},
			{EOWNERDEAD,     ERROR_ACCESS_DENIED},
			//{EPROTO,
			{EPROTONOSUPPORT,WSAEPROTONOSUPPORT},
			{EPROTOTYPE,     WSAEPROTOTYPE},
			{ETIME,          WSAETIMEDOUT},
			{ETIMEDOUT,      WSAETIMEDOUT},
			{ETXTBSY,        ERROR_BUSY},
			{EWOULDBLOCK,    WSAEWOULDBLOCK}
		};

		int i;

		for(i=0; i < countof(errmap); i++)
			if(err == errmap[i].err)
				return errmap[i].winerr;

		return ERROR_UNKNOWN;
	}
