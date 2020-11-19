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

#include "_process.h"
#include <pcl/error.h>
#include <pcl/tchar.h>

#ifdef PCL_DARWIN
#	include <libproc.h> // proc_pidpath
#elif defined(PCL_WINDOWS)
#	include <psapi.h> // GetModuleFileNameEx
#endif

int
pcl_proc_path(pid_t pid, tchar_t *buf, size_t len)
{
	int r=0;

	if(pid < 0)
		pid = pcl_proc_self();

#ifdef PCL_WINDOWS
	if(pid == pcl_proc_self())
	{
  	r = (int)GetModuleFileName(NULL, buf, (DWORD)len);
    if(r == 0 || GetLastError() == ERROR_INSUFFICIENT_BUFFER)
    	return 0;
	}
	else
	{
		HANDLE h = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);

		if(!h)
			return SETLASTERR();

		if(!(r = (int)GetModuleFileNameEx(h, NULL, buf, (DWORD)len)))
			r = SETLASTERR();

		CloseHandle(h);
	}

#elif defined(PCL_LINUX)
	char exe[80];

	sprintf(exe, "/proc/%d/exe", pid);
	r = (int)readlink(exe, buf, len - 1);
	if(r > -1 && r < (int)len)
		buf[r] = 0;
	else
		r = SETLASTERR();

#elif defined(PCL_DARWIN)
	char path[PROC_PIDPATHINFO_SIZE]; /* proc_pidpath is very picky about buf sizes! */

	if((r = proc_pidpath(pid, path, PROC_PIDPATHINFO_SIZE)) == 0)
		r = SETLASTERR();
	else if(r >= (int)len)
		r = SETERR(PCL_EBUF);
	else
		pcl_tcsncpy(buf, len, path, r);

#else
	r = SETERR(PCL_ENOIMPL);
#endif

	return r == -1 ? 0 : r;
}

