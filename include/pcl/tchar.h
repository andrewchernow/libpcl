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

#ifndef LIBPCL_TCHAR_H__
#define LIBPCL_TCHAR_H__

#include <pcl/string.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PCL_WINDOWS
	#ifndef _T
		#define _T(quote)                    L##quote /* may not have included correct Windows header yet */
	#endif
	typedef wchar_t tchar_t;
#	define PCL_TPATHSEP                   PCL_WPATHSEP
#	define PCL_TPATHSEPCHAR               PCL_WPATHSEPCHR
#	define pcl_totupper                   towupper
#	define pcl_tcslen                     wcslen
#	define pcl_tmemcpy                    wmemcpy
#	define pcl_tmemmove                   wmemmove
#	define pcl_tcscmp                     wcscmp
#	define pcl_tcsncmp                    wcsncmp
#	define pcl_tcsicmp                    pcl_wcsicmp
#	define pcl_tcsnicmp                   pcl_wcsnicmp
#	define pcl_tcsncpy                    pcl_wcsncpy
#	define pcl_tcsncat                    pcl_wcsncat
#	define pcl_tcscatf                    pcl_wcscatf
#	define pcl_vtcscatf                   pcl_vwcscatf
#	define pcl_tcssplit                   pcl_wcssplit
#	define pcl_tcsisplit                  pcl_wcsisplit
#	define pcl_tcschr                     wcschr
#	define pcl_tcsrchr                    wcsrchr
#	define pcl_tcsrepchr                  pcl_wcsrepchr
#	define pcl_tcsstr                     wcsstr
#	define pcl_tcsistr                    pcl_wcsistr
#	define pcl_tcstrim                    pcl_wcstrim
#	define pcl_tcstrunc                   pcl_wcstrunc
#	define pcl_tcsdup                     pcl_wcsdup
#	define pcl_tcsdup_s                   pcl_wcsdup_s
#	define pcl_tcsndup                    pcl_wcsndup
#	define pcl_tprintf                    pcl_wprintf
#	define pcl_ftprintf                   pcl_fwprintf
#	define pcl_vftprintf                  pcl_vfwprintf
#	define pcl_stprintf                   pcl_swprintf
#	define pcl_astprintf                  pcl_aswprintf
#	define pcl_vstprintf                  pcl_vswprintf
#	define pcl_vastprintf                 pcl_vaswprintf
#	define pcl_tcstoi                     pcl_wcstoi
#	define pcl_tcstoui                    pcl_wcstoui
#	define pcl_tcstol                     pcl_wcstol
#	define pcl_tcstoul                    pcl_wcstoul
#	define pcl_tcstoll                    pcl_wcstoll
#	define pcl_tcstoull                   pcl_wcstoull
#	define pcl_tcsisuuid                  pcl_wcsisuuid
#	define pcl_tcsupper                   pcl_wcsupper
#	define pcl_tcslower                   pcl_wcslower
#	define pcl_tmain                      wmain
#else
	typedef char tchar_t;
#	define PCL_TPATHSEP                   PCL_PATHSEP
#	define PCL_TPATHSEPCHAR               PCL_PATHSEPCHR
#	define _T(quote)                      quote /* windows defines this but not on unix */
#	define pcl_totupper                   toupper
#	define pcl_tcslen                     strlen
#	define pcl_memcpy                     memcpy
#	define pcl_tmemmove                   memmove
#	define pcl_tcscmp                     strcmp
#	define pcl_tcsncmp                    strncmp
#	define pcl_tcsicmp                    pcl_stricmp
#	define pcl_tcsnicmp                   pcl_strnicmp
#	define pcl_tcsncpy                    pcl_strncpy
#	define pcl_tcsncat                    pcl_strncat
#	define pcl_tcscatf                    pcl_strcatf
#	define pcl_vtcscatf                   pcl_vstrcatf
#	define pcl_tcssplit                   pcl_strsplit
#	define pcl_tcsisplit                  pcl_strisplit
#	define pcl_tcschr                     strchr
#	define pcl_tcsrchr                    strrchr
#	define pcl_tcsrepchr                  pcl_strrepchr
#	define pcl_tcsstr                     strstr
#	define pcl_tcsistr                    pcl_stristr
#	define pcl_tcstrim                    pcl_strtrim
#	define pcl_tcstrunc                   pcl_strtrunc
#	define pcl_tcsdup                     pcl_strdup
#	define pcl_tcsdup_s                   pcl_strdup_s
#	define pcl_tcsndup                    pcl_strndup
#	define pcl_tprintf                    pcl_printf
#	define pcl_ftprintf                   pcl_fprintf
#	define pcl_vftprintf                  pcl_vfprintf
#	define pcl_stprintf                   pcl_sprintf
#	define pcl_astprintf                  pcl_asprintf
#	define pcl_vstprintf                  pcl_vsprintf
#	define pcl_vastprintf                 pcl_vasprintf
#	define pcl_tcstoi                     pcl_strtoi
#	define pcl_tcstoui                    pcl_strtoui
#	define pcl_tcstol                     pcl_strtol
#	define pcl_tcstoul                    pcl_strtoul
#	define pcl_tcstoll                    pcl_strtoll
#	define pcl_tcstoull                   pcl_strtoull
#	define pcl_tcsupper                   pcl_strupper
#	define pcl_tcslower                   pcl_strlower
#	define pcl_tmain                      main
#endif

#define pcl_tcscpy(dest, size, src) pcl_tcsncpy(dest,size,src,0)
#define pcl_tcscat(dest, size, src) pcl_tcsncat(dest,size,src,0)
#define pcl_tmalloc(cch) (tchar_t *)pcl_malloc((cch) * sizeof(tchar_t))
#define pcl_trealloc(ptr, cch) (tchar_t *)pcl_realloc(ptr, (cch) * sizeof(tchar_t))

/* skip space and horizontal tabs */
PCL_INLINE tchar_t *
pcl_tskipblanks(const tchar_t *s)
{
	for(; *s && (*s == ' ' || *s == '\t'); s++);
	return (tchar_t *) s;
}

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_TCHAR_H__
