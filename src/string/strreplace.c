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

#include "_string.h"
#include <pcl/alloc.h>
#include <pcl/buf.h>

#undef xstrncmp
#undef xstrnicmp
#undef XBufText
#ifdef XWIDE
#	define xstrncmp wcsncmp
#	define xstrnicmp pcl_wcsnicmp
#	define XBufText PclBufTextW
#else
#	define xstrncmp strncmp
#	define xstrnicmp pcl_strnicmp
#	define XBufText PclBufText
#endif

static xchar *
XFUNC(replace_impl)(const xchar *s, const xchar *from, const xchar *to, int casefold)
{
	int to_len = (int) xstrlen(to), from_len = (int) xstrlen(from);

	/* use stack buffer so we can just steal it's data member */
	pcl_buf_t buf;
	pcl_buf_init(&buf, 64, XBufText);

	while(*s)
	{
		int n = casefold ? xstrnicmp(s, from, from_len) : xstrncmp(s, from, from_len);

		if(n == 0)
		{
			pcl_buf_put(&buf, to, to_len);
			s += from_len;
		}
		else
		{
			pcl_buf_putchar(&buf, *s++);
		}
	}

	return (xchar *) buf.data;
}

xchar *
XFUNC(replace)(const xchar *subject, const xchar *from, const xchar *to)
{
	return XFUNC(replace_impl)(subject, from, to, 0);
}

xchar *
XFUNC(ireplace)(const xchar *subject, const xchar *from, const xchar *to)
{
	return XFUNC(replace_impl)(subject, from, to, 1);
}

#ifndef XWIDE
#	define XWIDE
#	include "strreplace.c"
#endif
