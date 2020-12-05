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

#include "_process.h"
#include <pcl/error.h>
#include <pcl/limits.h> // PCL_MAXLONGPATH
#include <pcl/alloc.h>
#include <pcl/string.h>

/* Ofcourse, Windows requires a mini program with two completely different implementations:
 * one for the current process and another for the remote process. Uh vey. Like getting a
 * process's image path is some kind of top secret operation.
 */

static tchar_t *
remote_proc_path(pid_t pid)
{
	tchar_t *path = NULL;

	/* ignore error. Setting this allows access to local system processes. However, if we
	 * can't enable it, we can still open many other remote processes.
	 */
	bool enabled_priv = pcl_proc_setpriv(PRIV_PROCESS, true) == 0;

	/* Open the remote process */
	HANDLE hproc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);

	if(!hproc)
	{
		SETLASTERR();
		goto CLEANUP;
	}

	for(DWORD size = 64; size <= PCL_MAXLONGPATH + 1; size *= 2)
	{
		path = pcl_trealloc(path, size);

		DWORD len = size;
		BOOL ok = QueryFullProcessImageName(hproc, 0, path, &len);

		if(ok)
			break;

		if(pcl_oserrno != ERROR_INSUFFICIENT_BUFFER)
		{
			SETLASTERR();
			path = pcl_free(path);
			break;
		}
	}

CLEANUP:
	if(hproc)
		CloseHandle(hproc);

	if(enabled_priv)
		FREEZE_ERR(pcl_proc_setpriv(PRIV_PROCESS, false));

	return path;
}

static tchar_t *
current_proc_path()
{
	tchar_t *path = NULL;

	for(DWORD size = 64; size <= PCL_MAXLONGPATH + 1; size *= 2)
	{
		path = pcl_trealloc(path, size);

		DWORD len = GetModuleFileName(NULL, path, size);

		/* Docs indicate a return of zero is a bust */
		if(len == 0)
			break;

		/* Docs state if function succeeds, it returns char len excluding NUL. If buf is too small,
		 * it returns size and sets ERROR_INSUFFICIENT_BUFFER. Thus, if return value is greater than
		 * zero but less than size, it worked.
		 */
		if(len < size)
			return path;

		/* 'len' MUST be size here, only remaining possibility. In this case, last os error
		 * should be ERROR_INSUFFICIENT_BUFFER. If not, GetModuleFileName docs are wrong.
		 * This is a sanity check and should never happen.
		 */
		if(pcl_oserrno != ERROR_INSUFFICIENT_BUFFER)
			break;
	}

	pcl_free_safe(path);
	return R_SETLASTERR(NULL);
}

tchar_t *
pcl_proc_path(pid_t pid)
{
	tchar_t *path = pid < 0 ? current_proc_path() : remote_proc_path(pid);

	if(!path)
		TRC();

	return path;
}
