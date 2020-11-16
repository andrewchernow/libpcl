
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

#ifndef LIBPCL_STRING_H
#define LIBPCL_STRING_H

#include <string.h>
#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 *
 * @param buf
 * @param bufl
 * @param nbytes
 * @param decimal
 * @return
 */
PCL_EXPORT char *pcl_strfbytes(char *buf, size_t bufl, uint64_t nbytes, int decimal);

/**
 *
 * @param buf
 * @param bufl
 * @param nbytes
 * @param decimal
 * @return
 */
PCL_EXPORT wchar_t *pcl_wcsfbytes(wchar_t *buf, size_t bufl, uint64_t nbytes, int decimal);
PCL_EXPORT ssize_t pcl_strhexenc(char *buf, size_t size, const void *data, size_t len);
PCL_EXPORT ssize_t pcl_wcshexenc(wchar_t *buf, size_t size, const void *data, size_t len);
PCL_EXPORT ssize_t pcl_strhexdec(void *buf, size_t size, const char *str);
PCL_EXPORT ssize_t pcl_wcshexdec(void *buf, size_t size, const wchar_t *str);

#ifdef PCL_WINDOWS
#	define pcl_stricmp stricmp
#	define pcl_wcsicmp wcsicmp
#	define pcl_strnicmp strnicmp
#	define pcl_wcsnicmp wcsnicmp
#else
#	define pcl_stricmp strcasecmp
#	define pcl_wcsicmp wcscasecmp
#	define pcl_strnicmp strncasecmp
#	define pcl_wcsnicmp wcsncasecmp
#endif

#define pcl_strcpy(dest, size, src) pcl_strncpy(dest,size,src,0)
#define pcl_wcscpy(dest, size, src) pcl_wcsncpy(dest,size,src,0)
PCL_EXPORT char *pcl_strncpy(char *dest, size_t size, const char *src, size_t len);
PCL_EXPORT wchar_t *pcl_wcsncpy(wchar_t *dest, size_t size, const wchar_t *src, size_t len);

#ifdef PCL_LINUX
#	define pcl_stristr strcasestr
#	define pcl_wcsistr wcscasestr
#else
	PCL_EXPORT char *pcl_stristr(const char *haystack, const char *needle);
	PCL_EXPORT wchar_t *pcl_wcsistr(const wchar_t *haystack, const wchar_t *needle);
#endif

#define pcl_strdup(s) pcl_strndup(s, 0)
#define pcl_wcsdup(s) pcl_wcsndup(s, 0)
#define pcl_strdup_s(s) (s) ? pcl_strdup(s) : NULL
#define pcl_wcsdup_s(s) (s) ? pcl_wcsdup(s) : NULL
PCL_EXPORT char *pcl_strndup(const char *s, size_t len);
PCL_EXPORT wchar_t *pcl_wcsndup(const wchar_t *s, size_t len);


#define pcl_strcat(dest, size, src) pcl_strncat(dest,size,src,0)
#define pcl_wcscat(dest, size, src) pcl_wcsncat(dest,size,src,0)
PCL_EXPORT char *pcl_strncat(char *dest, size_t size, const char *src, size_t len);
PCL_EXPORT wchar_t *pcl_wcsncat(wchar_t *dest, size_t size, const wchar_t *src, size_t len);
PCL_EXPORT char *pcl_strcatf(char *dest, size_t size, const char *format, ...);
PCL_EXPORT wchar_t *pcl_wcscatf(wchar_t *dest, size_t size, const wchar_t *format, ...);
PCL_EXPORT char *pcl_vstrcatf(char *dest, size_t size, const char *format, va_list ap);
PCL_EXPORT wchar_t *pcl_vwcscatf(wchar_t *dest, size_t size, const wchar_t *format, va_list ap);
PCL_EXPORT char *pcl_astrcatf(const char *src, const char *format, ...);
PCL_EXPORT wchar_t *pcl_awcscatf(const wchar_t *src, const wchar_t *format, ...);
PCL_EXPORT char *pcl_vastrcatf(const char *src, const char *format, va_list ap);
PCL_EXPORT wchar_t *pcl_vawcscatf(const wchar_t *src, const wchar_t *format, va_list ap);


#define pcl_strltrim(s) pcl_strtrimset(s, NULL, true, false) /* trim left only */
#define pcl_wcsltrim(s) pcl_wcstrimset(s, NULL, true, false) /* trim left only */
#define pcl_strrtrim(s) pcl_strtrimset(s, NULL, false, true) /* trim right only */
#define pcl_wcsrtrim(s) pcl_wcstrimset(s, NULL, false, true) /* trim right only */
#define pcl_strtrim(s)  pcl_strtrimset(s, NULL, true, true)  /* trim both */
#define pcl_wcstrim(s)  pcl_wcstrimset(s, NULL, true, true)  /* trim both */
PCL_EXPORT char *pcl_strtrimset(char *s, const char *set, bool ltrim, bool rtrim);
PCL_EXPORT wchar_t *pcl_wcstrimset(wchar_t *s, const wchar_t *set, bool ltrim, bool rtrim);

PCL_EXPORT char *pcl_strtrunc(char *dest, size_t size, const char *src);
PCL_EXPORT wchar_t *pcl_wcstrunc(wchar_t *dest, size_t size, const wchar_t *src);

PCL_EXPORT const char *pcl_strsignal(int signum);

/* str_replace. returns new length of in */
PCL_EXPORT int pcl_strrep(char *src, size_t src_max, const char *from, const char *to);
PCL_EXPORT int pcl_wcsrep(wchar_t *src, size_t src_max, const wchar_t *from, const wchar_t *to);
/* returns a newly allocated string that must be freed */
PCL_EXPORT char *pcl_astrrep(char *in, const char *from, const char *to);
PCL_EXPORT wchar_t *pcl_awcsrep(wchar_t *in, const wchar_t *from, const wchar_t *to);

/* returned vector is only NULL if given string is empty or NULL */
PCL_EXPORT pcl_vector_t *pcl_wcssplit(const wchar_t *s, const wchar_t *delim);
PCL_EXPORT pcl_vector_t *pcl_wcsisplit(const wchar_t *s, const wchar_t *delim);
PCL_EXPORT pcl_vector_t *pcl_strsplit(const char *s, const char *delim);
PCL_EXPORT pcl_vector_t *pcl_strisplit(const char *s, const char *delim);

PCL_EXPORT char *pcl_strlower(char *s);
PCL_EXPORT wchar_t *pcl_wcslower(wchar_t *s);
PCL_EXPORT char *pcl_strupper(char *s);
PCL_EXPORT wchar_t *pcl_wcsupper(wchar_t *s);

PCL_INLINE char *
pcl_strrepchr(char *s, int oldch, int newch)
{
	for(char *p = s; *p; p++)
		if(*p == oldch)
			*p = (char) newch;
	return s;
}

PCL_INLINE wchar_t *
pcl_wcsrepchr(wchar_t *s, wchar_t oldch, wchar_t newch)
{
	for(wchar_t *p = s; *p; p++)
		if(*p == oldch)
			*p = newch;
	return s;
}

#ifdef __cplusplus
}
#endif

#endif
