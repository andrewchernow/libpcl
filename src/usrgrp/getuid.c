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

#include "_usrgrp.h"
#include <pcl/error.h>
#include <pcl/string.h>

// note that uidp can be NULL for use as an exists check
int
pcl_getuid(const char *name, uid_t *uidp)
{
	int n;

	if(strempty(name))
		return BADARG();

#ifdef PCL_WINDOWS
	char *s = NULL;
	DWORD size = SECURITY_MAX_SID_SIZE;
	char buf[SECURITY_MAX_SID_SIZE];
	char dom[512];
	DWORD dom_len = 512;
	PSID sid = (PSID) buf;
	SID_NAME_USE snu;

	if(!LookupAccountNameA(NULL, name, sid, &size, dom, &dom_len, &snu))
	{
		n = SETLASTERR();
	}
	else if(!ConvertSidToStringSidA(sid, &s))
	{
		n = SETLASTERR();
	}
	else
	{
		if(uidp)
			pcl_strcpy(*uidp, sizeof(uid_t), s);
		n = 0;
	}

	if(s)
		LocalFree(s);

#else
	char buf[4096];
	struct passwd pwbuf, *pw = NULL;

	n = getpwnam_r(name, &pwbuf, buf, sizeof(buf), &pw);

	if(n == 0)
	{
		if(!pw)
			n = SETERR(PCL_ENOTFOUND);
		else if(uidp)
			*uidp = pw->pw_uid;
	}
	else
	{
		n = SETOSERR(n);
	}
#endif

	return n;
}
