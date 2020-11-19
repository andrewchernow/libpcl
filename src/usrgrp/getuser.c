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

int
getuser(const uid_t uid, char *out, size_t len)
{
	int n;

	if(!out || len == 0)
		return BADARG();

	*out = 0;

#ifdef PCL_WINDOWS
	PSID sid = NULL;

	if(!uid[0])
		n = BADARG();
	else if(!ConvertStringSidToSidA(uid, &sid))
		n = SETLASTERR();
	else
		n = ipcl_win32_sid_to_name(sid, out, len);

	if(sid)
		LocalFree(sid);

#else
	char buf[4096];
	struct passwd pwbuf, *pw=NULL;

	n = getpwuid_r(uid, &pwbuf, buf, sizeof(buf), &pw);

	if(n == 0)
	{
		if(!pw)
			n = SETERR(PCL_ENOTFOUND);
		else if(!pcl_strcpy(out, len, pw->pw_name))
			n = TRC();
	}
	else
	{
		n = SETOSERR(n);
	}
#endif

	return n;
}
