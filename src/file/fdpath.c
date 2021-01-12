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

#include "_file.h"
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/io.h>
#include <pcl/string.h>

#ifdef PCL_DARWIN
#	include <fcntl.h>
#	include <sys/param.h>
#elif defined(PCL_WINDOWS)
#	include <Psapi.h>
#	include <io.h>
#	include <pcl/limits.h>
typedef DWORD (* GetFinalPathNameByHandle_t)(HANDLE,LPTSTR,DWORD,DWORD);
#else
#	include <limits.h>
#endif

int
pcl_fdpath(int fd, pchar_t **out)
{
	int out_len = 0;

	if(!out || fd < 0)
		return BADARG();

#ifdef PCL_WINDOWS
	HANDLE hfile = (HANDLE)_get_osfhandle(fd);
	GetFinalPathNameByHandle_t getpath = (GetFinalPathNameByHandle_t)GetProcAddress(
		GetModuleHandleA("kernel.dll"), "GetFinalPathNameByHandle");

	if(getpath)
	{
		pchar_t tmp[1], *result = NULL;
		DWORD dw = getpath(hfile, tmp, countof(tmp), FILE_NAME_NORMALIZED);

		/* function failed */
		if(dw == 0)
			return SETLASTERR();

		result = pcl_pmalloc(dw);
		dw = getpath(hfile, result, dw-1, FILE_NAME_NORMALIZED);

		if(dw == 0)
		{
			pcl_free(result);
			return SETLASTERR();
		}

		/* Remove the longpath "\\?\" prefix */
		if(!pcl_pcsnicmp(result, _P("\\\\?\\"), 4))
		{
			memmove(result, result + 4, ((dw - 4) + 1) * sizeof(pchar_t));
			dw -= 4;
		}

		*out = result;
		out_len = (int)(dw - 1); /* includes NUL, remove it */
	}
	/* GetFinalPathNameByHandle() function appeared in Vista. */
	else
	{
		HANDLE hmap;
		void *addr;
		DWORD szhi = 0;
		pchar_t devpath[512 + PCL_MAXPATH];
		bool found = false;
		pchar_t drivebuf[1024];
		pchar_t *drive = drivebuf;
		DWORD szlow = GetFileSize(hfile, &szhi);

		/* Can't map zero byte file */
		if(szhi == 0 && szlow == 0)
			return SETERR(PCL_ENOTSUP);

		if(!(hmap = CreateFileMapping(hfile, NULL, PAGE_READONLY, 0, 1, NULL)))
			return SETLASTERR();

		if(!(addr = MapViewOfFile(hmap, FILE_MAP_READ, 0, 0, 1)))
		{
			FREEZE_ERR(CloseHandle(hmap));
			return SETLASTERR();
		}

		/* this gives us a device path, that's okay */
		if(!GetMappedFileName(GetCurrentProcess(), addr, devpath, countof(devpath)))
		{
			FREEZE_ERR(
				UnmapViewOfFile(addr);
				CloseHandle(hmap);
			);

			return SETLASTERR();
		}

		UnmapViewOfFile(addr);
		CloseHandle(hmap);

		/* GetMappedFilename returns device paths:
		 *   C:\WINNT would be \Device\Harddisk0\Partition1\WINNT
		 * Convert to drive letter path.
		 */
		if(!GetLogicalDriveStrings(countof(drivebuf)-1, drivebuf))
			return SETLASTERR();

		do
		{
			pchar_t name[1024];

			if(QueryDosDevice(drive, name, countof(name)))
			{
				size_t nlen = pcl_pcslen(name);

				if(nlen < countof(name) && !pcl_pcsnicmp(devpath, name, nlen) &&
					devpath[nlen] == _P('\\'))
				{
					out_len = pcl_aspprintf(out, _P("%Ps%Ps"), drive, devpath + nlen);
					if(out_len < 0)
						return TRC();

					found = true;
					break;
				}
			}

			drive += (pcl_pcslen(drive) + 1);
		}
		while(*drive);

		/* use device path if no drive letter mapping was found */
		if(!found)
		{
			*out = pcl_pcsdup(devpath);
			out_len = (int)pcl_pcslen(*out);
		}
	}

#elif defined(PCL_DARWIN)
	char pbuf[MAXPATHLEN + 1];

	if(fcntl(fd, F_GETPATH, pbuf) == -1)
		return SETLASTERR();

	*out = pcl_pcsdup(pbuf);
	out_len = (int) pcl_pcslen(*out);

#else
	char fdpath[128];
	char pbuf[PATH_MAX + 1];

	pcl_sprintf(fdpath, sizeof(fdpath), "/proc/%d/fd/%d", getpid(), fd);
	out_len = (int) readlink(fdpath, pbuf, sizeof(pbuf));
	if(out_len == -1)
		return SETLASTERR();

	*out = pcl_strndup(pbuf, out_len); /* NUL terminates for us */
#endif

	return out_len;
}
