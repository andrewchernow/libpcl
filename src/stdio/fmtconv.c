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

/* this function is hard to look at, sorry. A primary goal was to avoid
 * allocation and make this as fast as possible. In addition, the function
 * must deal with converting a uniform format string (PCL's input) into
 * one of at least 3 different platform format strings...mostly due to windows
 * changing format type specs based on printf or wprintf...OMG!
 *
 * Anyway, this creates a platform-specific format string given an PCL format
 * string. The returned string might be a pointer to 'buf' (which is most likely
 * stack) or this function might allocate a new buffer if more room is needed.
 * The callee must check if the provided 'buf' argument address matches the
 * returned address when this function returns. If it doesn't match, the callee
 * must free() the returned pointer.
 */

#include "../string/_string.h"
#include <pcl/stdlib.h>
#include <ctype.h>

#undef XPATHSEPCHAR
#undef toxupper

#ifdef XWIDE
#define XPATHSEPCHAR PCL_WPATHSEPCHR
#define toxupper towupper
#else
#define XPATHSEPCHAR PCL_PATHSEPCHR
#define toxupper toupper
#endif

xchar *
XFUNC(fmtconv)(xchar *buf, size_t bufsz, const xchar *pcl_format)
{
	xchar *os_format = buf;
	xchar *out = os_format;
	size_t len = xstrlen(pcl_format) + 1;

	/* allocate when needed */
	if(len > bufsz)
	{
		os_format = (xchar *) pcl_malloc(len * sizeof(xchar));
		out = os_format;
	}

	while(*pcl_format)
	{
		xchar *p = xstrchr(pcl_format, X('%'));
		size_t siz = len - (out - os_format);

		/* done */
		if(!p)
		{
			len = xstrlen(pcl_format);
			xstrncpy(out, siz, pcl_format, len);
			out += len;
			break;
		}

		/* move past % so its included in copy */
		p++;
		xstrncpy(out, siz, pcl_format, p - pcl_format);

		/* adjust ptr positions */
		out += (p - pcl_format);
		pcl_format = p;

		/* special case, "%%" to encode a '%'. The below while loop won't handle
		 * this properly so we handle it here. If the below loop encounters a
		 * percent, it knows to exit the below loop and wrap around to the outer
		 * loop'out strchr('%').
		 */
		if(*pcl_format == X('%'))
		{
			pcl_format++; /* no copy, already have 1st '%', just move past 2nd one */
			continue;
		}

		/* scan for end of format spec */
		int finished = 0;
		while(!finished && *pcl_format)
		{
			switch(*pcl_format)
			{
				case X('%'):
					/* don't copy, no format advancement and let strchr in outer loop
					 * handle this. This is either a new format or a user-end typo.
					 */
					finished = 1;
					break;

					/* "%/" path sep: PCL extension */
				case X('/'):
				{
					/* '/' must appear directly after '%' */
					p = (xchar *) pcl_format - 1;

					if(*p == X('%'))
					{
						out--; /* overwrite previous '%' */
						*out++ = XPATHSEPCHAR;
						pcl_format++; /* skip the '/' */
					}
					else
					{
						/* '/' is an invalid spec char, copy it and finish */
						finished = 1;
						*out++ = *pcl_format++;
					}

					break;
				}

					/* "%ts|%tc" tchar_t: PCL extension (supports upper/lower 'T') */
				case X('t'):
				case X('T'):
				{
					xchar save_c = *pcl_format++;
					xchar c = *pcl_format;

					/* %ts always wide on Windows, always single byte on unixes */
					if(c == X('s') || c == X('c'))
					{
#ifdef PCL_WINDOWS
#	ifdef XWIDE
						*out++ = c; /* lower s,c is wide for wprintf funcs */
#	else
						*out++ = (xchar) toupper(c); /* upper S,C is wide for printf funcs */
#	endif
#else
						*out++ = c; /* lower s,c always single byte */
#endif //PCL_WINDOWS

						pcl_format++; /* skip out or c */
					}
					else
					{
						*out++ = save_c; /* consume t|T, invalid spec char */
					}

					/* either had %ts or %Tc or invalid spec char */
					finished = 1;
					break;
				}

					/* convert %ls and %lc to windows format specifiers. Handle on windows
					 * only. Notice 'l' and 'h' field width fall-thru to the catch all
					 * when not on windows.
					 */
				case X('h'):
				case X('l'):
#ifdef PCL_WINDOWS
				{
					xchar c = pcl_format[1];

					if(c == X('s') || c == X('c'))
					{
#ifdef XWIDE
						*out++ = c; /* lower out,c is wide for wprintf funcs */
#else
						*out++ = (xchar) toxupper(c); /* upper S,C is wide for printf funcs */
#endif

						pcl_format += 2;

					}
					else
					{
						*out++ = *pcl_format++;
					}

					break;
				}

				case X('s'):
				case X('c'):
				{
					xchar c = *pcl_format++;

#ifdef XWIDE
					c = (xchar)toupper(c);
#endif

					*out++ = c;
				}
#endif //PCL_WINDOWS

					/* flags, field width or precision */
				case X('0'):
				case X('1'):
				case X('2'):
				case X('4'):
				case X('5'):
				case X('6'):
				case X('7'):
				case X('8'):
				case X('9'):
				case X('-'):
				case X(' '):
				case X('+'):
				case X('\''):
				case X('*'):
				case X('.'):
				case X('L'):
				case X('q'):
				case X('j'):
				case X('z'):
					*out++ = *pcl_format++;
					break;

					/* we either have a conversion specifier (d, i, s, etc...) or an
					 * invalid spec char. Either way, copy char to the buffer
					 * and mark as finished.
					 */
				default:
					*out++ = *pcl_format++;
					finished = 1;
					break;
			}
		}
	}

	*out = 0;
	return os_format;
}

#ifndef XWIDE
#define XWIDE
#include "fmtconv.c"
#endif
