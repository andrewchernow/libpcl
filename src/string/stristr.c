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

#include "_string.h"

/* Windows doesn't have strcasestr or stristr, you have to use shlwapi.h StrStrIA and StrStrIW
 * which are very slow for some reason. Also requires unwanted linking with Shlwapi.lib.
 * In addition, Darwin doesn't have wcscasestr but does have strcasestr. For simplicity,
 * we replace darwin's strcasestr with the below since these funcs come in pairs.
 */

#undef toxlower

#ifdef XWIDE
#	include <wctype.h>
	#define toxlower towlower
#else
#	include <ctype.h>
	#define toxlower tolower
#endif

xchar *
XFUNC(istr)(const xchar *haystack, const xchar *needle)
{
	xint c = toxlower((xint) *needle);

	if(c == '\0')
		return (xchar *) haystack;

	for(; *haystack; haystack++)
	{
		if(toxlower((xint) *haystack) == c)
		{
			for(size_t i = 0;;)
			{
				if(needle[++i] == '\0')
					return (xchar *) haystack;

				if(toxlower((xint) haystack[i]) != toxlower((xint) needle[i]))
					break;
			}
		}
	}

	return NULL;
}

#ifndef XWIDE
	#define XWIDE
	#include "stristr.c"
#endif
