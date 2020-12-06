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
#include <pcl/io.h>

#ifdef PCL_WINDOWS
#	include <windows.h>
#	include <pcl/alloc.h>

static wchar_t *
win32_utf8towide(const char *src, size_t len, size_t *lenp)
{
	if(!*src)
		return pcl_wcsdup(L"");

	int out_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, (int) len, NULL, 0);

	if(out_len == 0 || out_len == 0xFFFD)
		return NULL;

	wchar_t *out = pcl_tmalloc(out_len + 1);
	out_len = MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, src, (int) len, out, out_len);

	out[out_len] = 0;
	if(lenp)
		*lenp = out_len;

	return out;
}
#endif

tchar_t *
pcl_utf8_to_tcs(const char *src, size_t src_len, size_t *lenp)
{
	tchar_t * s;

	if(!src)
		return R_SETERR(NULL, PCL_EINVAL);

	if(src_len == 0)
		src_len = strlen(src);

#ifdef PCL_WINDOWS
	s = win32_utf8towide(src, src_len, lenp);
#else
	s = pcl_strndup(src, src_len);
	if(lenp)
		*lenp = src_len;
#endif

	return s;
}