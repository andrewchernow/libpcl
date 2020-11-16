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

int
ipcl_win32_sd_clear(SECURITY_DESCRIPTOR *sd)
{
	if(!sd)
		return 0; // silently ignore

	DWORD rev;
	SECURITY_DESCRIPTOR_CONTROL flags;

	/* NOTE: the "sd" MUST be absolute: ie NOT self-referencing */
	if(GetSecurityDescriptorControl(sd, &flags, &rev) && (flags & SE_SELF_RELATIVE))
		return SETLASTERRMSG("security descriptor must be absolute", 0);

	BOOL u, u2;
	PSID sid;
	PACL acl;

	if(GetSecurityDescriptorOwner(sd, &sid, &u) && sid)
		pcl_free(sid);

	if(GetSecurityDescriptorGroup(sd, &sid, &u) && sid)
		pcl_free(sid);

	if(GetSecurityDescriptorDacl(sd, &u2, &acl, &u) && u2 && acl)
		pcl_free(acl);

	if(GetSecurityDescriptorSacl(sd, &u2, &acl, &u) && u2 && acl)
		pcl_free(acl);

	/* once we free each part of the descriptor, reinitialize it */
	ipcl_win32_sd_init(sd);

	return 0;
}
