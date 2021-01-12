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

PSID
ipcl_win32_name_to_sid(const pchar_t *name, int *type)
{
	PSID sid;
	DWORD sidlen = 0;
	SID_NAME_USE sid_type;
	pchar_t domain[512];
	DWORD domain_size = countof(domain);

	if(strempty(name))
		return R_SETERR(NULL, PCL_EINVAL);

	if(!LookupAccountName(NULL, name, NULL, &sidlen, domain, &domain_size, &sid_type) &&
		GetLastError() != ERROR_INSUFFICIENT_BUFFER)
	{
		return R_SETLASTERR(NULL);
	}

	domain_size = countof(domain);
	sid = (PSID) pcl_malloc(sidlen);

	if(!LookupAccountName(NULL, name, sid, &sidlen, domain, &domain_size, &sid_type))
	{
		pcl_free(sid);
		SETLASTERR();
		return NULL;
	}

	if(type)
		*type = sid_type;

	return sid;
}
