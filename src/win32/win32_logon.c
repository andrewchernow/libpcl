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
#include <pcl/string.h>

int
ipcl_win32_logon(const pchar_t *username, const pchar_t *domain,
	const pchar_t *password, HANDLE *htok)
{
	if(!htok)
		return SETERR(PCL_EINVAL);

	/*
	 * When on a workgroup trying to logon as a domain account, you must
	 * use LOGON32_LOGON_NEW_CREDENTIALS with provider set to
	 * LOGON32_PROVIDER_WINNT50.  When on a domain using a domain
	 * account, LOGON32_LOGON_INTERACTIVE or LOGON32_LOGON_BATCH with
	 * provider set to LOGON32_PROVIDER_DEFAULT.  The latter also applies
	 * to any situation (workgroup, domain, stand-alone server) where the
	 * account is a local account ... not a remote domain account.
	 *
	 * We try both since determining all of the above is much more expensive
	 * than trying LogonUser two times.
	 */
	if(!LogonUser(username, domain, password, LOGON32_LOGON_INTERACTIVE,
		LOGON32_PROVIDER_DEFAULT, htok))
	{
		if(!LogonUser(username, domain, password, LOGON32_LOGON_NEW_CREDENTIALS,
			LOGON32_PROVIDER_WINNT50, htok))
		{
			return SETLASTERRMSG("Cannot logon account '%Ps\\%Ps'",
				domain ? domain : _P(""), username);
		}
	}

	return 0;
}
