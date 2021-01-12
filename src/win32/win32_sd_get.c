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

#include "_win32.h"
#include <pcl/alloc.h>

SECURITY_DESCRIPTOR *
sd_get(const pchar_t *path, DWORD *len, int sec_info)
{
	DWORD size;
	SECURITY_DESCRIPTOR *sd;
	BOOL success;
	HANDLE file = CreateFile(path, READ_CONTROL | FILE_READ_ATTRIBUTES | GENERIC_READ | GENERIC_WRITE,
		0, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_ATTRIBUTE_NORMAL, NULL);

	if(file == INVALID_HANDLE_VALUE)
		return R_SETLASTERR(NULL);

	/* query for size */
	GetKernelObjectSecurity(file, sec_info, NULL, 0, &size);
	if(GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		CloseHandle(file);
		return R_SETLASTERR(NULL);
	}

	/* Allocate SECURITY_DESCRIPTOR and query for data */
	sd = (SECURITY_DESCRIPTOR *) pcl_malloc(size);
	success = GetKernelObjectSecurity(file, sec_info, sd, size, &size); // self-relative format

	CloseHandle(file);

	if(!success)
	{
		pcl_free(sd);
		return R_SETLASTERR(NULL);
	}

	if(len)
		*len = (DWORD) size;

	return sd;
}

int
ipcl_win32_sd_get(const pchar_t *path, SECURITY_DESCRIPTOR **sd, DWORD *sdlen,
	PSID *world_sid, PSID *owner_sid, PSID *group_sid, BOOL *have_dacl, PACL *dacl,
	SECURITY_INFORMATION secinfo)
{
	BOOL u;

	*have_dacl = false;

	if(world_sid)
		*world_sid = ipcl_win32_world_sid();

	if(!(*sd = sd_get(path, sdlen, secinfo)))
	{
		FreeSid(*world_sid);
		return TRC();
	}

	if((secinfo & OWNER_SECURITY_INFORMATION) && owner_sid &&
		 !GetSecurityDescriptorOwner(*sd, owner_sid, &u))
	{
		pcl_free(*sd);
		FreeSid(*world_sid);
		return SETLASTERR();
	}

	if((secinfo & GROUP_SECURITY_INFORMATION) && group_sid &&
		 !GetSecurityDescriptorGroup(*sd, group_sid, &u))
	{
		pcl_free(*sd);
		FreeSid(*world_sid);
		return SETLASTERR();
	}

	if(!dacl)
		return 0;

	*dacl = NULL;

	if(!GetSecurityDescriptorDacl(*sd, have_dacl, dacl, &u))
	{
		pcl_free(*sd);
		FreeSid(*world_sid);
		return SETLASTERR();
	}
	else if(!*dacl)
	{
		*have_dacl = false;
	}

	return 0;
}


