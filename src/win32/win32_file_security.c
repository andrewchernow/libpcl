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

#include "_win32.h"
#include <pcl/stat.h>
#include <pcl/alloc.h>

int
ipcl_win32_file_security(const pchar_t *path, mode_t *mode, uid_t *uidp, gid_t *gidp)
{
	ACL *dacl;
	BOOL have_dacl;
	DWORD sdlen;
	SECURITY_DESCRIPTOR *sd;
	PSID owner_sid=NULL, group_sid=NULL, world_sid=NULL;
	int secinfo = DACL_SECURITY_INFORMATION | OWNER_SECURITY_INFORMATION | GROUP_SECURITY_INFORMATION;

	if(!path || !mode)
		return SETERR(PCL_EINVAL);

	*mode = 0;

	if(ipcl_win32_sd_get(path, &sd, &sdlen, &world_sid, &owner_sid, &group_sid,
		&have_dacl, &dacl, secinfo))
		return TRC();

	if(uidp)
	{
		char *s;
		if(ConvertSidToStringSidA(owner_sid, &s))
		{
			pcl_strtrunc(*uidp, sizeof(uid_t), s);
			LocalFree(s);
		}
	}

	if(gidp)
	{
		char *s;
		if(ConvertSidToStringSidA(group_sid, &s))
		{
			pcl_strtrunc(*gidp, sizeof(gid_t), s);
			LocalFree(s);
		}
	}

	if(!have_dacl)
	{
		*mode = S_IRWXU | S_IRWXG | S_IRWXO;
		pcl_free(sd);
		FreeSid(world_sid);
		return 0;
	}

	for(DWORD i = 0; i < dacl->AceCount; i++)
	{
		ACCESS_ALLOWED_ACE *ace;
		PSID ace_sid;

		if(!GetAce(dacl, i, (PVOID * ) & ace))
			continue;

		if(ace->Header.AceFlags & INHERIT_ONLY_ACE)
			continue;

		ace_sid = (PSID) & ace->SidStart;

		if(ace->Header.AceType == ACCESS_ALLOWED_ACE_TYPE)
		{
			if(EqualSid(owner_sid, ace_sid))
				*mode |= ipcl_win32_rights_to_mode(ace->Mask, S_IRUSR);
			else if(EqualSid(group_sid, ace_sid))
				*mode |= ipcl_win32_rights_to_mode(ace->Mask, S_IRGRP);
			else if(EqualSid(world_sid, ace_sid))
				*mode |= ipcl_win32_rights_to_mode(ace->Mask, S_IROTH);
		}
	}

	pcl_free(sd);
	FreeSid(world_sid);
	return 0;
}
