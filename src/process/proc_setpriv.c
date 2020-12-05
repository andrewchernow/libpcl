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

int
pcl_proc_setpriv(const tchar_t *priv, bool enable)
{
#ifdef PCL_WINDOWS
	LUID luid;
	HANDLE htok = NULL;
	DWORD access = TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES;
	HANDLE hproc = OpenProcess(PROCESS_QUERY_INFORMATION, false, pcl_proc_self());

	if(!hproc)
		return SETLASTERR();

	if(!OpenProcessToken(hproc, access, &htok))
	{
		CloseHandle(hproc);
		return SETLASTERR();
	}

	bool okay;
	if((okay = LookupPrivilegeValue(NULL, priv, &luid)))
	{
		TOKEN_PRIVILEGES tp;
		tp.PrivilegeCount           = 1;
		tp.Privileges[0].Luid       = luid;
		tp.Privileges[0].Attributes = enable ? SE_PRIVILEGE_ENABLED : 0;
		okay = AdjustTokenPrivileges(htok, FALSE, &tp, sizeof(tp), NULL, NULL);
	}

	if(!okay)
		SETLASTERR();

	if(hproc)
		CloseHandle(hproc);

	CloseHandle(htok);

	return okay ? 0 : -1;

#else
	UNUSED(priv || enable);
	return 0;
#endif
}
