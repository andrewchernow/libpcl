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

#include "_win32.h"
#include <pcl/stat.h>

#define CLEANUP do{ \
	pcl_free_s(owner_sid); \
	pcl_free_s(group_sid); \
	if(world_sid) \
		FreeSid(world_sid); \
	pcl_free_s(dacl); \
}while(0)

int
ipcl_win32_sd_create(SECURITY_DESCRIPTOR *sd, const tchar_t *owner,
	const tchar_t *group, mode_t mode)
{
	DWORD acl_len;
	PACL dacl = NULL;
	PSID owner_sid = NULL;
	PSID group_sid = NULL;
	PSID world_sid = NULL;

	ipcl_win32_sd_init(sd);

	if(mode & S_IRWXU)
	{
		if(!strempty(owner))
		{
			int type;

			if((owner_sid = ipcl_win32_name_to_sid(owner, &type)) && type != SidTypeUser)
			{
				pcl_free(owner_sid);
				owner_sid = NULL;
			}
		}

		/* no owner provided or it was not found, use process owner */
		if(!owner_sid)
		{
			HANDLE htok;

			if(ipcl_win32_process_token(&htok) == 0)
			{
				TOKEN_USER *tokUser;

				if(ipcl_win32_token_into(htok, TokenUser, &tokUser) == 0)
				{
					owner_sid = ipcl_win32_sid_dup(tokUser->User.Sid);
					pcl_free(tokUser);
				}

				CloseHandle(htok);
			}
		}

		if(owner_sid && !SetSecurityDescriptorOwner(sd, owner_sid, false))
		{
			pcl_free(owner_sid);
			return SETLASTERR();
		}
	}

	if(mode & S_IRWXG)
	{
		if(!strempty(group))
		{
			int type;

			if((group_sid = ipcl_win32_name_to_sid(group, &type)) && !SID_ISGROUP(type))
			{
				pcl_free(group_sid);
				group_sid = NULL;
			}
		}

		/* no group provided or it was not found, use process group */
		if(!group_sid)
		{
			HANDLE htok;

			if(ipcl_win32_process_token(&htok) == 0)
			{
				TOKEN_PRIMARY_GROUP *tokGroup;

				if(ipcl_win32_token_into(htok, TokenPrimaryGroup, &tokGroup) == 0)
				{
					group_sid = ipcl_win32_sid_dup(tokGroup->PrimaryGroup);
					pcl_free(tokGroup);
				}

				CloseHandle(htok);
			}
		}

		if(group_sid && !SetSecurityDescriptorGroup(sd, group_sid, false))
		{
			SetSecurityDescriptorOwner(sd, NULL, false);
			pcl_free(owner_sid);
			pcl_free(group_sid);
			return SETLASTERR();
		}
	}

	if(mode & S_IRWXO)
		world_sid = ipcl_win32_world_sid();

	acl_len = sizeof(ACL);

	if(owner_sid)
		acl_len += (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(owner_sid));

	if(group_sid)
		acl_len += (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(group_sid));

	if(world_sid)
		acl_len += (sizeof(ACCESS_ALLOWED_ACE) - sizeof(DWORD) + GetLengthSid(world_sid));

	dacl = (PACL) pcl_malloc(acl_len);

	if(!InitializeAcl(dacl, acl_len, ACL_REVISION))
	{
		CLEANUP;
		return SETLASTERR();
	}

	if(owner_sid)
	{
		if(!AddAccessAllowedAceEx(dacl, ACL_REVISION, OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE,
			ipcl_win32_mode_to_rights(mode, S_IRUSR), owner_sid))
		{
			CLEANUP;
			return SETLASTERR();
		}
	}

	if(group_sid)
	{
		if(!AddAccessAllowedAceEx(dacl, ACL_REVISION, OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE,
			ipcl_win32_mode_to_rights(mode, S_IRGRP), group_sid))
		{
			CLEANUP;
			return SETLASTERR();
		}
	}

	if(world_sid)
	{
		if(!AddAccessAllowedAceEx(dacl, ACL_REVISION, OBJECT_INHERIT_ACE | CONTAINER_INHERIT_ACE,
			ipcl_win32_mode_to_rights(mode, S_IROTH), world_sid))
		{
			CLEANUP;
			return SETLASTERR();
		}
	}

	if(!SetSecurityDescriptorDacl(sd, true, dacl, false))
	{
		CLEANUP;
		return SETLASTERR();
	}

	/* NO CLEANUP HERE! Note that SetSecurityDescriptorDacl is specifically documented to require
	 * an "absolute" descriptor, meaning its members are pointers to structures rather than
	 * being self-referencing (offsets to contiguous data). This is why each SID is allocated.
	 * For the security descriptor to be usable after this call, we can't free any *_sid or the
	 * DACL. Again SetSecurityDescriptorDacl docs state "The DACL is referenced by, not copied
	 * into, the security descriptor". One must call ipcl_win32_sd_clear to cleanup properly.
	 *
	 * The exception: world_sid is only used by AddAccessAllowedAceEx and is not referenced by the
	 * created SECURITY_DESCRIPTOR.
	 */

	if(world_sid)
		FreeSid(world_sid);

	return 0;
}
