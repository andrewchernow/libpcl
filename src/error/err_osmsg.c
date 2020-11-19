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

#include "_error.h"
#include <string.h>

#ifdef PCL_WINDOWS
#	include <windows.h>
#endif

int
pcl_err_osmsg(uint32_t oserr, char *buf, size_t size)
{
	if(!buf || !size)
		return 0;

	*buf = 0;
	if(oserr == 0)
		return 0;

#ifdef PCL_WINDOWS
	DWORD save_err = GetLastError();
	DWORD flags = FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS;
	DWORD len = FormatMessageA(flags, NULL, oserr, 0, buf, (DWORD)min(size, 65535), NULL);

	/* Not in system table, try looking up error in Ntdsbmsg.dll. */
	if(len == 0)
	{
		HMODULE hinst = LoadLibraryA("ntdsbmsg.dll");
		if(hinst)
		{
			flags = FORMAT_MESSAGE_FROM_HMODULE | FORMAT_MESSAGE_IGNORE_INSERTS;
			len = FormatMessageA(flags, hinst, oserr, 0, buf, (DWORD)size, NULL);
			FreeLibrary(hinst);
		}
	}

	SetLastError(save_err);

	if(len > 0)
	{
		/* remove possible CRLF or LF */
		if(len >= 2 && buf[len - 2] == '\r')
			len-=2, buf[len] = 0;
		else if(len >= 1 && buf[len - 1] == '\n')
			buf[--len] = 0;
	}

#else
	int e = errno; // ignore possible errors of strerror_r

#	ifdef PCL_DARWIN
	// XSI-compliant version
	(void) strerror_r(oserr, buf, size);
#	else
	// linux has XSI-compliant but not when _GUN_SOURCE is defined, which PCL defines
	char *x = strerror_r(oserr, buf, size);

	/* Oddly enough, strerror_r can return its own static or thread_local pointer and never use
	 * the provided user buffer. Easy enough to test by comparing pointers.
	 */
	if(x != buf)
		pcl_strcpy(buf, size, x);
#	endif

	errno = e;
	size_t len = strlen(buf);
#endif

	return (int) len;
}
