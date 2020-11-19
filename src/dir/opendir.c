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

#ifdef PCL_WINDOWS
#	include <pcl/stdlib.h>
#endif

DIR *
pcl_opendir(const tchar_t *path)
{
	pcl_err_osclear();

#ifdef PCL_WINDOWS
	tchar_t buf[16 * 1024];
	DIR *dp = (DIR *)pcl_malloc(sizeof(DIR));

	if(!pcl_tcscmp(path, _T(".")))
	{
		buf[0] = _T('*');
		buf[1] = 0;
	}
	else
	{
		size_t n = pcl_tcslen(path);

		pcl_tcscpy(buf, countof(buf), path);

		if(buf[n - 1] != PCL_TPATHSEPCHAR)
			buf[n++] = PCL_TPATHSEPCHAR;

		buf[n++] = _T('*');
		buf[n] = 0;
	}

	if((dp->hfind = FindFirstFile(buf, &dp->wfd)) == INVALID_HANDLE_VALUE)
	{
		SETLASTERRMSG("Cannot open directory: %ts", path);
		pcl_free(dp);
		return NULL;
	}

	dp->findnextfile = false;

#else
	DIR *dp = opendir(path);

	if(!dp)
	{
		SETLASTERRMSG("Cannot open directory: %ts", path);
		return NULL;
	}
#endif

	return dp;
}
