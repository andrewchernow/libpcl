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

#include <pcl/error.h>
#include <pcl/string.h>

#ifdef PCL_WINDOWS
#	include <windows.h>
#	include <pcl/alloc.h>
#else
#	include <limits.h>
#	include <stdlib.h>
#endif

tchar_t *
pcl_realpath(const tchar_t *path, int *lenp)
{
	tchar_t *fullpath;

	if(lenp)
		*lenp = 0;

	if(strempty(path))
		return R_SETERR(NULL, PCL_EINVAL);

#ifdef PCL_WINDOWS
	/* query for size: len will include NUL byte in this case */
	DWORD len = GetFullPathName(path, 0, NULL, NULL);

	if(len == 0)
		return R_SETLASTERR(NULL);

	fullpath = pcl_malloc(len * sizeof(tchar_t));

	/* call again with allocated buffer */
	len = GetFullPathName(path, len, fullpath, NULL);

	if(len == 0)
		return R_SETLASTERR(NULL);

#else
	char pbuf[PATH_MAX];

	if(!realpath((char *) path, pbuf))
		return R_SETLASTERR(NULL);

	size_t len = strlen(pbuf);
	fullpath = pcl_strndup(pbuf, len);
#endif

	if(lenp)
		*lenp = (int) len;

	return fullpath;
}
