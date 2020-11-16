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

#include <pcl/stdlib.h>
#include <pcl/error.h>
#include <string.h>

#ifdef PCL_WINDOWS
	#include <windows.h>
#else
	#include <limits.h>
#endif

int
pcl_realpath(tchar_t *out, int outl, const tchar_t *path)
{
	if(!out || outl < 1 || strempty(path))
		return BADARG();

	int len;
#ifdef PCL_WINDOWS
	tchar_t *p;
	len = (int)GetFullPathName(path, (DWORD)outl, out, &p);

	if(len == 0)
		return SETLASTERR();

	if(len >= outl)
		return SETERR(PCL_EBUF);

#else
	char pbuf[PATH_MAX];

	if(!realpath((char *) path, pbuf))
		return SETLASTERR();

	len = (int) strlen(pbuf);
	if(outl <= len)
		return SETERR(PCL_EBUF);

	memcpy(out, pbuf, len + 1);
#endif

	return len;
}
