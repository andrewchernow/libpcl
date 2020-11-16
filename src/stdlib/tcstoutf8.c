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

#include <pcl/types.h>
#include <pcl/error.h>
#include <pcl/tchar.h>
#include <pcl/stdlib.h>

#ifdef PCL_WINDOWS
#	include <windows.h>

static char *
win32_wcstoutf8(const wchar_t *src, size_t len, size_t *lenp)
{
	if(!src)
		return NULL;

	if(!*src)
		return pcl_strdup("");

	if(!len)
		len = wcslen(src);

	int utf8_len = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, src, (int) len,
		NULL, 0, NULL, NULL);

	if(utf8_len == 0 || utf8_len == 0xfffd)
		return NULL;

	char *utf8 = (char *) pcl_malloc(utf8_len + 1);
	utf8_len = WideCharToMultiByte(CP_UTF8, MB_ERR_INVALID_CHARS, src, (int) len, utf8,
		utf8_len, NULL, NULL);

	if(utf8_len == 0)
	{
		pcl_free(utf8);
		return NULL;
	}

	utf8[utf8_len] = 0;
	if(lenp)
		*lenp = utf8_len;

	return utf8;
}
#endif

char *
pcl_tcstoutf8(const tchar_t *src, size_t src_len, size_t *lenp)
{
	char *s;

	if(!src)
		return R_SETERR(NULL, PCL_EINVAL);

	if(src_len == 0)
		src_len = pcl_tcslen(src);

#ifdef PCL_WINDOWS
	s = win32_wcstoutf8(src, src_len, lenp);
#else
	s = pcl_tcsndup(src, src_len);
	if(lenp)
		*lenp = src_len;
#endif

	return s;
}
