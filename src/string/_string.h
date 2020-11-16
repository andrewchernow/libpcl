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

/* Each string source file includes this header twice: once without XWIDE defined and
 * again with it defined. For one-time initializations, like includes, use the NOT definied
 * XWIDE case below.
 */

#undef XFUNC
#undef X
#undef xchar
#undef xstrlen
#undef xstrstr
#undef xstristr
#undef xstrdup
#undef xstrndup
#undef xstrchr
#undef xstrncpy
#undef vxprintf
#undef xfmtconv
#undef vfxprintf
#undef xint

#ifdef XWIDE
#	define XFUNC(name) pcl_wcs ## name
#	define X(s) L ## s
#	define xchar wchar_t
#	define xstrlen wcslen
#	define xstrstr wcsstr
#	define xstristr pcl_wcsistr
#	define xstrdup(s) pcl_wcsndup(s,0)
#	define xstrndup pcl_wcsndup
#	define xstrchr wcschr
#	define xstrncpy pcl_wcsncpy
#	define vxprintf pcl_vswprintf
#	define xfmtconv pcl_wcsfmtconv
#	define vfxprintf pcl_vfwprintf
#	define xint wint_t
#else
#	define XFUNC(name) pcl_str ## name
#	define X(s) s
#	define xchar char
#	define xstrlen strlen
#	define xstrstr strstr
#	define xstristr pcl_stristr
#	define xstrdup(s) pcl_strndup(s,0)
#	define xstrndup pcl_strndup
#	define xstrchr strchr
#	define xstrncpy pcl_strncpy
#	define vxprintf pcl_vsprintf
#	define xfmtconv pcl_strfmtconv
#	define vfxprintf pcl_vfprintf
#	define xint int
#	include <pcl/stdio.h>
#	include <pcl/string.h>
#	include <pcl/error.h>
#	include <stdarg.h>
#	ifdef _WIN32
#		define ssize_t intptr_t
#	endif
#endif

/* not exported */
xchar *XFUNC(fmtconv)(xchar *buf, size_t bufsz, const xchar *pcl_format);
