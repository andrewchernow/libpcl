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

#ifndef XWIDE
#	include <pcl/array.h>
#	include <pcl/alloc.h>
#endif

static pcl_array_t *
XFUNC(strsplit_impl)(const xchar *s, const xchar *delim, int casefold)
{
	size_t delim_len = xstrlen(delim);
	pcl_array_t *arr = pcl_array(4, pcl_cleanup_ptr);
	xchar *(*fn_strstr)(const xchar *, const xchar *) = casefold ? xstristr : xstrstr;

	while(s && *s)
	{
		xchar *elem, *p = fn_strstr(s, delim);

		if(!p)
		{
			elem = xstrdup(s);
			s = NULL;
		}
		else
		{
			elem = xstrndup(s, p - s);
			s = p + delim_len;
		}

		pcl_array_append(arr, elem);
	}

	return arr;
}

pcl_array_t *
XFUNC(split)(const xchar *s, const xchar *d)
{
	return XFUNC(strsplit_impl)(s, d, 0);
}

pcl_array_t *
XFUNC(isplit)(const xchar *s, const xchar *d)
{
	return XFUNC(strsplit_impl)(s, d, 1);
}

#ifndef XWIDE
#	define XWIDE
#	include "strsplit.c"
#endif
