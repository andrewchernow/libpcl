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
#include "../win32/_win32.h"
#include <pcl/error.h>

int
ipcl_file_open(pcl_file_t *file, const tchar_t *path, int pcl_oflags, mode_t mode)
{
	DWORD a = INVALID_FILE_ATTRIBUTES;

	/* -------------------------------------------------------------------------
	 * Portability Implementations for O_DIRECTORY and O_NOFOLLOW
	 */

	if(pcl_oflags & PCL_O_NOFOLLOW)
	{
		WIN32_FIND_DATA wfd;
		HANDLE hfind = FindFirstFile(path, &wfd);

		if(hfind != INVALID_HANDLE_VALUE)
		{
			FindClose(hfind);
			a = wfd.dwFileAttributes;

			if((a & FILE_ATTRIBUTE_REPARSE_POINT) && (wfd.dwReserved0 & IO_REPARSE_TAG_SYMLINK))
				return SETERRMSG(PCL_ETYPE, "%ts is a symbolic link", path);
		}
	}

	/* implement O_DIRECTORY: fail if file doesn't exist, fail if not a dir */
	if(pcl_oflags & PCL_O_DIRECTORY)
	{
		/* don't call getfileattrs if we just called findfirstfile. if file
		 * doesn't exist, fail since O_DIRECTORY is set
		 */
		if(a == INVALID_FILE_ATTRIBUTES && (a = GetFileAttributes(path)) == INVALID_FILE_ATTRIBUTES)
			return SETLASTERR();

		/* file is not a dir, punt */
		if(!(a & FILE_ATTRIBUTE_DIRECTORY))
			return SETERR(PCL_ENOTDIR);
	}


	/* -------------------------------------------------------------------------
	 * Share Mode
	 */

	DWORD share_mode;

	if(pcl_oflags & PCL_O_SHLOCK)
	{
		share_mode = FILE_SHARE_READ;
		file->flags |= PCL_FF_OLOCK;
	}
	else if(pcl_oflags & PCL_O_EXLOCK)
	{
		share_mode = 0;
		file->flags |= PCL_FF_OLOCK;
	}
	else
	{
		share_mode = FILE_SHARE_READ | FILE_SHARE_WRITE;
	}


	/* -------------------------------------------------------------------------
	 * File Access
	 */

	DWORD access = READ_CONTROL | FILE_READ_ATTRIBUTES | GENERIC_READ;

	if(pcl_oflags & PCL_O_RDWR)
		access |= (GENERIC_READ | GENERIC_WRITE);
	else if(pcl_oflags & PCL_O_WRONLY)
		access |= GENERIC_WRITE;


	/* -------------------------------------------------------------------------
	 * Create Disposition
	 */

	DWORD create_mode = 0;

	switch(pcl_oflags & (PCL_O_CREAT | PCL_O_EXCL | PCL_O_TRUNC))
	{
		case 0:
		case PCL_O_EXCL: /* ignore EXCL w/o CREAT */
			create_mode = OPEN_EXISTING;
			break;

		case PCL_O_CREAT:
			create_mode = OPEN_ALWAYS;
			break;

		case PCL_O_CREAT | PCL_O_EXCL:
		case PCL_O_CREAT | PCL_O_TRUNC | PCL_O_EXCL:
			create_mode = CREATE_NEW;
			break;

		case PCL_O_TRUNC:
		case PCL_O_TRUNC | PCL_O_EXCL: /* ignore EXCL w/o CREAT */
			create_mode = TRUNCATE_EXISTING;
			break;

		case PCL_O_CREAT | PCL_O_TRUNC:
			create_mode = CREATE_ALWAYS;
			break;

		default:
			return SETERRMSG(PCL_EINVAL, "Invalid open pcl_oflags: %08x", pcl_oflags);
	}


	/* -------------------------------------------------------------------------
	 * Flags and Attributes
	 */

	DWORD attrs = 0;

	/* see CreateFile docs which recommends mixing nobuf and overlapped pcl_oflags */
	if(pcl_oflags & PCL_O_NONBLOCK)
		attrs |= (FILE_FLAG_OVERLAPPED | FILE_FLAG_NO_BUFFERING);

	if(pcl_oflags & PCL_O_BACKUP)
		attrs |= FILE_FLAG_BACKUP_SEMANTICS;

	if((pcl_oflags & PCL_O_CREAT) && !(mode & 0222))
		attrs |= FILE_ATTRIBUTE_READONLY;
	else
		attrs |= FILE_ATTRIBUTE_NORMAL;

	/* See CreateFile for why write-through and nobuf should be enabled to
	 * emulate the Unix O_SYNC concept.
	 */
	if(pcl_oflags & PCL_O_SYNC)
		attrs |= (FILE_FLAG_WRITE_THROUGH | FILE_FLAG_NO_BUFFERING);


	/* -------------------------------------------------------------------------
	 * Security Attributes
	 */

	SECURITY_DESCRIPTOR sd;
	SECURITY_ATTRIBUTES sa = {0};
	sa.bInheritHandle = (pcl_oflags & PCL_O_NOINHERIT) ? false : true; /* O_CLOEXEC */
	sa.nLength = sizeof(sa);

	/* create secdescr w/mode (use calling process owner as file owner) */
	if(pcl_oflags & PCL_O_CREAT)
	{
		memset(&sd, 0, sizeof(sd));

		if(ipcl_win32_sd_create(&sd, NULL, NULL, mode))
			return TRC();

		sa.lpSecurityDescriptor = &sd;
	}

	/* finally, create the file */
	file->fd = CreateFile(path, access, share_mode, &sa, create_mode, attrs, NULL);

	if(file->fd == PCL_BADFD)
	{
		if(pcl_oserrno == ERROR_ACCESS_DENIED)
		{
			/* remove readonly attribute and try to open/create file again */
			if((a = GetFileAttributes(path)) != INVALID_FILE_ATTRIBUTES &&
				 (a & FILE_ATTRIBUTE_READONLY) && SetFileAttributes(path, a & ~FILE_ATTRIBUTE_READONLY))
			{
				file->fd = CreateFile(path, access, share_mode, &sa, create_mode, attrs, NULL);
			}
		}

		if(file->fd == PCL_BADFD)
		{
			if(sa.lpSecurityDescriptor)
				ipcl_win32_sd_clear(sa.lpSecurityDescriptor);

			return SETLASTERRMSG("%ts", path);
		}
	}

	if(sa.lpSecurityDescriptor)
		ipcl_win32_sd_clear(sa.lpSecurityDescriptor);

	return 0;
}
