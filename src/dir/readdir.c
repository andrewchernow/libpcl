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

#include "_dir.h"
#include <pcl/error.h>
#include <pcl/tchar.h>

bool
pcl_readdir(DIR *dp, pcl_dirent_t *ent)
{
	if(!dp || !ent)
		return R_SETERR(false, PCL_EINVAL);

	memset(ent, 0, sizeof(*ent));
	pcl_err_osclear();

#ifdef PCL_WINDOWS
	if(!dp->findnextfile)
	{
		dp->findnextfile = true;
		if(pcl_tcscmp(dp->wfd.cFileName, _T(".")) && pcl_tcscmp(dp->wfd.cFileName, _T("..")))
			goto FOUND_ENTRY;
	}

	if(dp->findnextfile)
	{
		while(true)
		{
			if(FindNextFile(dp->hfind, &dp->wfd))
			{
				if(!pcl_tcscmp(dp->wfd.cFileName, _T(".")) || !pcl_tcscmp(dp->wfd.cFileName, _T("..")))
					continue;
				break;
			}

			/* skip access denied */
			if(pcl_oserrno != ERROR_ACCESS_DENIED)
			{
				SETLASTERR();
				return false;
			}
		}
	}

	FOUND_ENTRY:;
	pcl_tcscpy(ent->name, countof(ent->name), dp->wfd.cFileName);
	ent->namlen = (uint16_t)pcl_tcslen(ent->name);

	if(dp->wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT)
		ent->type = DT_LNK;
	else if(dp->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		ent->type = DT_DIR;
	else
		ent->type = DT_REG;

#else
	while(true)
	{
		int n;
		char buffer[64 + DIRBLKSIZ + 1] = {0};
		struct dirent *e = (struct dirent *) buffer, *result = NULL;

		if((n = readdir_r(dp, e, &result)) != 0)
		{
			SETOSERR(n);
			return false;
		}

		/* nothing left in directory */
		if(!result)
			return R_SETERR(false, PCL_ENOMORE);

		/* do not report these */
		if(!pcl_tcscmp(e->d_name, _T(".")) || !pcl_tcscmp(e->d_name, _T("..")))
			continue;

		/* got an entry */
		ent->type = e->d_type;

		#ifdef PCL_DARWIN
		ent->namlen = e->d_namlen;
		#else
		ent->namlen = (uint16_t)strlen(e->d_name);
		#endif

		pcl_tcsncpy(ent->name, countof(ent->name), e->d_name, ent->namlen);
		break;
	}
#endif

	return true;
}
