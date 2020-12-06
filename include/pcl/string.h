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

#ifndef LIBPCL_STRING_H
#define LIBPCL_STRING_H

/** @defgroup string String Management
 * A set of string utility functions. The PCL string library includes many standard C string
 * functions for the sole purpose of providing buffer safe versions. There are other functions
 * that have no counterpart in the standard C library, but are a common need.
 *
 * Almost all PCL string functions come in two flavors: UTF-8 (char)
 * and UTF-16 (wchar_t). These two version exists to support PCL's concept of a ::tchar_t: a
 * \c char on Unixes and \c wchar_t on Windows. For example, the below shows some basic mappings:
 * @code
 * pcl_tcscpy   => Unix pcl_strcpy, Windows pcl_wcscpy
 * pcl_tcsdup   => Unix pcl_strdup, Windows pcl_wcsdup
 * pcl_tcscatf  => Unix pcl_strcatf, Windows pcl_wcscatf
 * @endcode
 * It is actually uncommon to use \c str or \c wcs versions directly in a cross-platform
 * application. It is much more common to use the \c tcs versions.
 * @{
 */
#include <pcl/types.h>
#include <string.h>
#ifdef PCL_UNIX
#	include <strings.h>
#endif
#include <ctype.h>
#include <wctype.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Format a number in byte units.
 * This uses multiples of 1024 and supports the following units: Kibibyte, Mebibyte, Gibibyte,
 * Tebibyte, Pebibyte, Exibyte. The formatted unit is appened to the converted number value
 * as a single uppercase character (no spaces): 10K, 50M, etc. For values less than 1024, no
 * unit is appened; this is just bytes.
 * @param buf pointer to a buffer to write formatted value
 * @param bufl character length of \a buf
 * @param nbytes number of bytes, value to format
 * @param decimal number of decimal places to include in the formatted value.
 * @return number of character written to \a buf or -1 on error.
 */
PCL_EXPORT int pcl_strfbytes(char *buf, size_t bufl, uint64_t nbytes, int decimal);
/** @copydoc pcl_strfbytes */
PCL_EXPORT int pcl_wcsfbytes(wchar_t *buf, size_t bufl, uint64_t nbytes, int decimal);

/** Encode data as a hexadecimal string.
 * @param buf pointer to a buffer
 * @param size character length of \a buf
 * @param data pointer to the data to encode
 * @param len number of \a data bytes to encode
 * @return number of characters written to \a buf or -1 on error
 */
PCL_EXPORT int pcl_hex_encode(char *buf, size_t size, const void *data, size_t len);

/** Decode a hexadecimal string.
 * @warning This does not add a NUL to \a buf
 * @param buf pointer to a buffer
 * @param size byte length of \a buf which should be at least \c strlen(hex) bytes
 * @param hex pointer to the hexadecimal string
 * @return number of bytes written to \a buf or -1 on error
 */
PCL_EXPORT int pcl_hex_decode(void *buf, size_t size, const char *hex);

/** Encode data as a base64 string.
 * @param buf pointer to a buffer
 * @param size character length of \a buf
 * @param data pointer to the data to encode
 * @param len number of \a data bytes to encode
 * @return number of characters written to \a buf or -1 on error
 */
PCL_EXPORT int pcl_base64_encode(char *buf, size_t size, const void *data, size_t len);

/** Decode a base64 string.
 * @param buf pointer to a buffer
 * @param size byte length of \a buf
 * @param b64 pointer to the base64 string
 * @return number of bytes written to \a buf or -1 on error
 */
PCL_EXPORT int pcl_base64_decode(void *buf, size_t size, const char *b64);

/* ------------------------------------------------------------------------------
 * pcl_stricmp, pcl_strnicmp, pcl_wcsicmp, pcl_wcsnicmp
 */

#ifdef __doxygen__
	/** Compare two strings ignoring case.
	 * @param a pointer to a string
	 * @param b pointer to a string
	 * @return an integer less than, equal to or greater than zero if \a a is found, respectively,
	 * to be less than, to match or be greater than \a b
	 * @note implemented as a macro
	 */
	int pcl_stricmp(const char *a, const char *b);
	/** @copydoc pcl_stricmp */
	int pcl_wcsicmp(const wchar_t *a, const wchar_t *b);

	/** Compare two strings ignoring case.
	 * @param a pointer to a string
	 * @param b pointer to a string
	 * @param n compare only the first N bytes
	 * @return an integer less than, equal to or greater than zero if \a a is found, respectively,
	 * to be less than, to match or be greater than \a b
	 * @note implemented as a macro
	 */
	int pcl_strnicmp(const char *a, const char *b, size_t n);
	/** @copydoc pcl_strnicmp */
	int pcl_wcsnicmp(const wchar_t *a, const wchar_t *b, size_t n);
#elif defined(PCL_WINDOWS)
#	define pcl_stricmp _stricmp
#	define pcl_wcsicmp _wcsicmp
#	define pcl_strnicmp _strnicmp
#	define pcl_wcsnicmp _wcsnicmp
#else
#	define pcl_stricmp strcasecmp
#	define pcl_wcsicmp wcscasecmp
#	define pcl_strnicmp strncasecmp
#	define pcl_wcsnicmp wcsncasecmp
#endif

/* ------------------------------------------------------------------------------
 * pcl_strcpy, pcl_strncpy, pcl_wcscpy, pcl_wcsncpy
 */

#ifdef __doxygen__
	/** Copy a string.
	 * @param dest pointer to a buffer
	 * @param size character length of \a dest
	 * @param src pointer to the string to copy
	 * @return pointer to \a dest or \c NULL on error
	 * @note implemented as a macro
	 */
	char *pcl_strcpy(char *dest, size_t size, const char *src);
	/** @copydoc pcl_strcpy */
	wchar_t *pcl_wcscpy(wchar_t *dest, size_t size, const wchar_t *src);
#else
#	define pcl_strcpy(dest, size, src) pcl_strncpy(dest,size,src,0)
#	define pcl_wcscpy(dest, size, src) pcl_wcsncpy(dest,size,src,0)
#endif

/** Copy a portion of a string.
 * @param dest pointer to a buffer
 * @param size character length of \a dest. one byte is always reserved for \c NUL.
 * @param src pointer to the string to copy
 * @param len number of characters in \a src to copy
 * @return pointer to \a dest or \c NULL on error.
 */
PCL_EXPORT char *pcl_strncpy(char *dest, size_t size, const char *src, size_t len);
/** @copydoc pcl_strncpy */
PCL_EXPORT wchar_t *pcl_wcsncpy(wchar_t *dest, size_t size, const wchar_t *src, size_t len);


/* ------------------------------------------------------------------------------
 * pcl_stristr and pcl_wcsistr
 */

#ifdef __doxygen__
	/** Locate a substring ignoring case.
	 * @param haystack pointer to a string to search within
	 * @param needle pointer to a string
	 * @return pointer to the first occurance of \a needle within \a haystack or \c NULL on error.
	 */
	char *pcl_stristr(const char *haystack, const char *needle);
	/** @copydoc pcl_stristr */
	wchar_t *pcl_wcsistr(const wchar_t *haystack, const wchar_t *needle);
#elif defined(PCL_UNIX)
#	define pcl_stristr strcasestr
#else
	PCL_EXPORT char *pcl_stristr(const char *haystack, const char *needle);
#endif
PCL_EXPORT wchar_t *pcl_wcsistr(const wchar_t *haystack, const wchar_t *needle);

/* ------------------------------------------------------------------------------
 * pcl_strdup, pcl_strndup, pcl_wcsdup, pcl_wcsndup
 */

#ifdef __doxygen__
	/** Duplicate a string.
	 * @param s pointer to string
	 * @return pointer to allocated duplicate
	 * @note implemented as a macro
	 */
	char *pcl_strdup(const char *s);
	/** @copydoc pcl_strdup */
	wchar_t *pcl_wcsdup(const wchar_t *s);
#else
#	define pcl_strdup(s) pcl_strndup(s, 0)
#	define pcl_wcsdup(s) pcl_wcsndup(s, 0)
#endif

/** Duplicate a portion of a string.
 * @param s pointer to string
 * @param len number of characters to duplicate
 * @return pointer to allocated duplicate
 */
PCL_EXPORT char *pcl_strndup(const char *s, size_t len);
/** @copydoc pcl_strndup */
PCL_EXPORT wchar_t *pcl_wcsndup(const wchar_t *s, size_t len);

/* ------------------------------------------------------------------------------
 * pcl_strcat family
 */

#ifdef __doxygen__
	/** Concatenate two strings.
	 * @param dest pointer to a buffer
	 * @param size character length of \a dest
	 * @param src pointer to the string to copy
	 * @return pointer to \a dest
	 * @note implemented as a macro
	 */
	char *pcl_strcat(char *dest, size_t size, const char *src);
	/** @copydoc pcl_strcat */
	wchar_t *pcl_wcscat(wchar_t *dest, size_t size, const wchar_t *src);
#else
#	define pcl_strcat(dest, size, src) pcl_strncat(dest,size,src,0)
#	define pcl_wcscat(dest, size, src) pcl_wcsncat(dest,size,src,0)
#endif

/** Concatenate two strings.
 * @param dest pointer to a buffer to append to
 * @param size character length of \a dest
 * @param src pointer to the string to copy
 * @param len character length of \a src to copy
 * @return pointer to \a dest
 */
PCL_EXPORT char *pcl_strncat(char *dest, size_t size, const char *src, size_t len);
/** @copydoc pcl_strncat */
PCL_EXPORT wchar_t *pcl_wcsncat(wchar_t *dest, size_t size, const wchar_t *src, size_t len);

/** Concatenate two strings using a format.
 * @param dest pointer to a buffer to append to
 * @param size character length of \a dest
 * @param format printf-style format string
 * @param ... variable arguments
 * @return pointer to \a dest
 */
PCL_EXPORT char *pcl_strcatf(char *dest, size_t size, const char *format, ...);
/** @copydoc pcl_strcatf */
PCL_EXPORT wchar_t *pcl_wcscatf(wchar_t *dest, size_t size, const wchar_t *format, ...);

/** Concatenate two strings using a format.
 * @param dest pointer to a buffer to append to
 * @param size character length of \a dest
 * @param format printf-style format string
 * @param ap variable argument list
 * @return pointer to \a dest
 */
PCL_EXPORT char *pcl_vstrcatf(char *dest, size_t size, const char *format, va_list ap);
/** @copydoc pcl_vstrcatf */
PCL_EXPORT wchar_t *pcl_vwcscatf(wchar_t *dest, size_t size, const wchar_t *format, va_list ap);

/** Allocate and concatenate two strings using a format.
 * @param src pointer to a string to to use as the first set of characters. This can be \c NULL.
 * @param format printf-style format string
 * @param ... variable arguments
 * @return pointer to an allocated string starting with \a src, if not \c NULL, and ending
 * with the formatted results.
 */
PCL_EXPORT char *pcl_astrcatf(const char *src, const char *format, ...);
/** @copydoc pcl_astrcatf */
PCL_EXPORT wchar_t *pcl_awcscatf(const wchar_t *src, const wchar_t *format, ...);

/** Allocate and concatenate two strings using a format.
 * @param src pointer to a string to use as the first set of characters. This can be \c NULL.
 * @param format printf-style format string
 * @param ap variable argument list
 * @return pointer to an allocated string starting with \a src, if not \c NULL, and ending
 * with the formatted results.
 */
PCL_EXPORT char *pcl_vastrcatf(const char *src, const char *format, va_list ap);
/** @copydoc pcl_vastrcatf */
PCL_EXPORT wchar_t *pcl_vawcscatf(const wchar_t *src, const wchar_t *format, va_list ap);

/* ------------------------------------------------------------------------------
 * pcl_strtrim family
 */

#ifdef __doxygen__
	/** Trim whitespace from the front and back of a string.
	 * @param s pointer to a string
	 * @return pointer to the \a s argument
	 * @note implemented as a macro
	 */
	char *pcl_strtrim(char *s);
	/** @copydoc pcl_strtrim */
	wchar_t *pcl_wcstrim(wchar_t *s);

	/** Trim whitespace from the front (left) of a string.
	 * @param s pointer to a string
	 * @return pointer to the \a s argument
	 * @note implemented as a macro
	 */
	char *pcl_strltrim(char *s);
	/** @copydoc pcl_strltrim */
	wchar_t *pcl_wcsltrim(wchar_t *s);

	/** Trim whitespace from the back (right) of a string.
	 * @param s pointer to a string
	 * @return pointer to the \a s argument
	 * @note implemented as a macro
	 */
	char *pcl_strrtrim(char *s);
	/** @copydoc pcl_strrtrim */
	wchar_t *pcl_wcsrtrim(wchar_t *s);
#else
#	define pcl_strltrim(s) pcl_strtrimset(s, NULL, true, false)
#	define pcl_wcsltrim(s) pcl_wcstrimset(s, NULL, true, false)
#	define pcl_strrtrim(s) pcl_strtrimset(s, NULL, false, true)
#	define pcl_wcsrtrim(s) pcl_wcstrimset(s, NULL, false, true)
#	define pcl_strtrim(s)  pcl_strtrimset(s, NULL, true, true)
#	define pcl_wcstrim(s)  pcl_wcstrimset(s, NULL, true, true)
#endif

/** Trim a set of characters from a string.
 * @param s pointer to a string
 * @param set a set of characters (as a string) to trim. If this is \c NULL, a default set
 * is used: \c " \t\n\v\f\r"
 * @param left true to trim from the front (left) of \a s
 * @param right true to trim from the back (right) of \a s
 * @return pointer to \a s
 */
PCL_EXPORT char *pcl_strtrimset(char *s, const char *set, bool left, bool right);
/** @copydoc pcl_strtrimset */
PCL_EXPORT wchar_t *pcl_wcstrimset(wchar_t *s, const wchar_t *set, bool left, bool right);

/* ------------------------------------------------------------------------------
 * pcl_strreplace family
 */

/** Replace all occurances of a search string with a replacement string.
 * @param subject pointer to source string
 * @param from pointer to the search string within \a subject
 * @param to pointer to the replacement string
 * @return an allocated string with all occurances of \a from replaced with \a to
 */
PCL_EXPORT char *pcl_strreplace(const char *subject, const char *from, const char *to);
/** @copydoc pcl_strreplace */
PCL_EXPORT wchar_t *pcl_wcsreplace(const wchar_t *subject, const wchar_t *from, const wchar_t *to);

/** Replace all occurances of a search string with a replacement string ignoring case.
 * @param subject pointer to source string
 * @param from pointer to the search string within \a subject
 * @param to pointer to the replacement string
 * @return an allocated string with all occurances of \a from replaced with \a to
 */
PCL_EXPORT char *pcl_strireplace(const char *subject, const char *from, const char *to);
/** @copydoc pcl_strireplace */
PCL_EXPORT wchar_t *pcl_wcsireplace(const wchar_t *subject, const wchar_t *from, const wchar_t *to);

/* ------------------------------------------------------------------------------
 * pcl_strsplit family
 */

/** Split a string into a vector.
 * @param s pointer to a string
 * @param delim pointer to a delimiter to split on
 * @return pointer to a vector containing the split parts. The delimiter string is never
 * included in part elements.
 */
PCL_EXPORT pcl_vector_t *pcl_strsplit(const char *s, const char *delim);
/** @copydoc pcl_strsplit */
PCL_EXPORT pcl_vector_t *pcl_wcssplit(const wchar_t *s, const wchar_t *delim);

/** Split a string into a vector ignoring case.
 * @param s pointer to a string
 * @param delim pointer to a delimiter to split on
 * @return pointer to a vector containing the split parts. The delimiter string is never
 * included in part elements.
 */
PCL_EXPORT pcl_vector_t *pcl_strisplit(const char *s, const char *delim);
/** @copydoc pcl_strisplit */
PCL_EXPORT pcl_vector_t *pcl_wcsisplit(const wchar_t *s, const wchar_t *delim);

/** Lowercase a string.
 * @param s pointer to a string
 * @return pointer to \a s in lowercase
 */
PCL_EXPORT char *pcl_strlower(char *s);
/** @copydoc pcl_strlower */
PCL_EXPORT wchar_t *pcl_wcslower(wchar_t *s);

/** Uppercase a string.
 * @param s pointer to a string
 * @return pointer to \a s in uppercase
 */
PCL_EXPORT char *pcl_strupper(char *s);
/** @copydoc pcl_strupper */
PCL_EXPORT wchar_t *pcl_wcsupper(wchar_t *s);

/** Copy and truncate a string. This will copy as much as possible. If \a dest is not
 * large enough, this will truncate \a src to fit within \a dest.
 * @param dest pointer to a buffer. if \a src can't fit, truncate
 * @param size character count of \a dest
 * @param src pointer to string to copy
 * @return pointer to \a dest
 */
PCL_EXPORT char *pcl_strtrunc(char *dest, size_t size, const char *src);
/** @copydoc pcl_strtrunc */
PCL_EXPORT wchar_t *pcl_wcstrunc(wchar_t *dest, size_t size, const wchar_t *src);

/** Extract a slice of a string. This is also known as \c substr.
 * @param s pointer to a string
 * @param pos starting character position
 * @param len number of characters to extract
 * @return pointer to an allocated slice of \a s
 */
PCL_EXPORT char *pcl_strslice(const char *s, size_t pos, size_t len);
/** @copydoc pcl_strslice */
PCL_EXPORT wchar_t *pcl_wcsslice(const wchar_t *s, size_t pos, size_t len);

/** Convert a signal number to a string.
 * @param signum signal number
 * @return pointer to a string
 */
PCL_EXPORT const char *pcl_strsignal(int signum);

/** Replace all occurance of a character in a string.
 * @param s pointer to a string
 * @param oldch search character
 * @param newch replacement character
 * @return pointer to \a s argument
 */
PCL_INLINE char *
pcl_strrepchr(char *s, int oldch, int newch)
{
	for(char *p = s; *p; p++)
		if(*p == oldch)
			*p = (char) newch;
	return s;
}

/** @copydoc pcl_strrepchr */
PCL_INLINE wchar_t *
pcl_wcsrepchr(wchar_t *s, wchar_t oldch, wchar_t newch)
{
	for(wchar_t *p = s; *p; p++)
		if(*p == oldch)
			*p = newch;
	return s;
}

/** Skip whitespace.
 * @param s pointer to a string
 * @return pointer to the first character in \a s that is not a whitespace character.
 */
PCL_INLINE char *
pcl_strskipws(const char *s)
{
	for(; isspace(*s); s++);
	return (char *) s;
}

/** @copydoc pcl_strskipws */
PCL_INLINE wchar_t *
pcl_wcsskipws(const wchar_t *s)
{
	for(; iswspace(*s); s++);
	return (wchar_t *) s;
}

/** Convert a tchar_t string to UTF-8. Internally, this is just a strdup on Unix
 * machines since a Unix tchar_t represents UTF-8.
 * @param src pointer to a tchar_t string
 * @param src_len byte length of the \a src argument. If this is zero, \c strlen is used
 * @param lenp If not \c NULL, character length will be written here
 * @return pointer to the result of conversion. This must be freed.
 */
PCL_EXPORT char *pcl_tcs_to_utf8(const tchar_t *src, size_t src_len, size_t *lenp);

/** Convert a UTF-8 string to a tchar_t string. Internally, this is just a strdup on Unix
 * machines since a Unix tchar_t represents UTF-8.
 * @param src pointer to a UTF-8 string
 * @param src_len byte length of the \a src argument. If this is zero, \c strlen is used
 * @param[out] lenp If not \c NULL, character length will be written here
 * @return pointer to the result of conversion. This must be freed.
 */
PCL_EXPORT tchar_t *pcl_utf8_to_tcs(const char *src, size_t src_len, size_t *lenp);

#ifdef __doxygen__
/** @defgroup tstring TCHAR Functions
 * The ::tchar_t functions map \c char or \c wchar_t string functions. They are all implemented as
 * macros but documented as functions. This makes the docs as clear as possible in regards to
 * return values, parameters, etc. These functions (macros) will map \c wchar_t versions on
 * Windows and \c char versions on Unixes. The \c wchar_t versions are UTF-16 while the \c char
 * versions are expected to be UTF-8. The ::tchar_t versions are prefixed with \c tcs or include
 * a \c t in the name (like ::pcl_totupper). The \c tcs prefix extends the \c wcs naming.
 * @{
 */

	/** Get a string's length
	 * @param s pointer to a string
	 * @return character length of string
	 * @note implemented as a macro
	 */
	size_t pcl_tcslen(const tchar_t *s);

	/** Find the first occurance of a character within a string.
	 * @param s pointer to a string
	 * @param c character to find
	 * @return first occurance of \a c within \a s or \c NULL if not found
	 * @note implemented as a macro
	 */
	tchar *pcl_tcschr(const tchar_t *s, tchar_t c);

	/** Find the last occurance of a character within a string.
	 * @param s pointer to a string
	 * @param c character to find
	 * @return last occurance of \a c within \a s or \c NULL if not found
	 * @note implemented as a macro
	 */
	tchar *pcl_tcsrchr(const tchar_t *s, tchar_t c);

	/** Uppercase a character.
	 * @param c character
	 * @return uppercase character
	 * @note implemented as a macro
	 */
	tchar_t pcl_totupper(tchar_t c);

	/** Lowercase a character.
	 * @param c character
	 * @return lowercase character
	 * @note implemented as a macro
	 */
	tchar_t pcl_totlower(tchar_t c);

	/** @copydoc pcl_strfbytes
	 * @note implemented as a macro
	 */
	int pcl_tcsfbytes(tchar_t *buf, size_t bufl, uint64_t nbytes, int decimal);

	/** Compare two strings.
	 * @param a pointer to a string
	 * @param b pointer to a string
	 * @return an integer less than, equal to or greater than zero if \a a is found, respectively,
	 * to be less than, to match or be greater than \a b
	 * @note implemented as a macro
	 */
	int pcl_tcscmp(const char *a, const char *b);

	/** Compare two strings.
	 * @param a pointer to a string
	 * @param b pointer to a string
	 * @param len number of characters to compare
	 * @return an integer less than, equal to or greater than zero if \a a is found, respectively,
	 * to be less than, to match or be greater than \a b
	 * @note implemented as a macro
	 */
	int pcl_tcsncmp(const char *a, const char *b, size_t len);

	/** @copydoc pcl_stricmp */
	int pcl_tcsicmp(const tchar_t *a, const tchar_t *b);

	/** @copydoc pcl_strnicmp */
	int pcl_tcsnicmp(const tchar_t *a, const tchar_t *b, size_t n);

	/** @copydoc pcl_strcpy */
	tchar_t *pcl_tcscpy(tchar_t *dest, size_t size, const tchar_t *src);

	/** @copydoc pcl_strncpy
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsncpy(tchar_t *dest, size_t size, const tchar_t *src, size_t len);

	/** Find the first occurance of a string within another string.
	 * @param haystack pointer to the source string
	 * @param needle pointer to the search string
	 * @return pointer to the first occurance of \a needle within \a haystack
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsstr(const tchar_t *haystack, const tchar_t *needle);

	/** Find the first occurance of a string within another string ignoring case.
	 * @param haystack pointer to the source string
	 * @param needle pointer to the search string
	 * @return pointer to the first occurance of \a needle within \a haystack
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsistr(const tchar_t *haystack, const tchar_t *needle);

	/** @copydoc pcl_strdup */
	tchar_t *pcl_tcsdup(const tchar_t *s);

	/** @copydoc pcl_strndup
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsndup(const tchar_t *s, size_t len);

	/** @copydoc pcl_strcat */
	tchar_t *pcl_tcscat(tchar_t *dest, size_t size, const tchar_t *src);

	/** @copydoc pcl_strncat
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsncat(tchar_t *dest, size_t size, const tchar_t *src, size_t len);

	/** @copydoc pcl_strcatf
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcscatf(tchar_t *dest, size_t size, const tchar_t *format, ...);

	/** @copydoc pcl_vstrcatf
	 * @note implemented as a macro
	 */
	tchar_t *pcl_vtcscatf(tchar_t *dest, size_t size, const tchar_t *format, va_list ap);

	/** @copydoc pcl_astrcatf
	 * @note implemented as a macro
	 */
	tchar_t *pcl_atcscatf(const tchar_t *src, const tchar_t *format, ...);

	/** @copydoc pcl_vastrcatf
	 * @note implemented as a macro
	 */
	tchar_t *pcl_vatcscatf(const tchar_t *src, const tchar_t *format, va_list ap);

	/** @copydoc pcl_strtrim */
	tchar_t *pcl_tcstrim(tchar_t *s);

	/** @copydoc pcl_strltrim */
	tchar_t *pcl_tcsltrim(tchar_t *s);

	/** @copydoc pcl_strrtrim */
	tchar_t *pcl_tcsrtrim(tchar_t *s);

	/** @copydoc pcl_strtrimset
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcstrimset(tchar_t *s, const tchar_t *set, bool left, bool right);

	/** @copydoc pcl_strreplace
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsreplace(const tchar_t *subject, const tchar_t *from, const tchar_t *to);

	/** @copydoc pcl_strireplace
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsireplace(const tchar_t *subject, const tchar_t *from, const tchar_t *to);

	/** @copydoc pcl_strsplit
	 * @note implemented as a macro
	 */
	pcl_vector_t *pcl_tcssplit(const tchar_t *s, const tchar_t *delim);

	/** @copydoc pcl_strisplit
	 * @note implemented as a macro
	 */
	pcl_vector_t *pcl_tcsisplit(const tchar_t *s, const tchar_t *delim);

	/** @copydoc pcl_strlower
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcslower(tchar_t *s);

	/** @copydoc pcl_strupper
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsupper(tchar_t *s);

	/** @copydoc pcl_strtrunc
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcstrunc(tchar_t *dest, size_t size, const tchar_t *src);

	/** @copydoc pcl_strslice
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsslice(const tchar_t *s, size_t pos, size_t len);

	/** @copydoc pcl_strrepchr
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsrepchr(tchar_t *s, tchar_t oldch, tchar_t newch);

	/** @copydoc pcl_strskipws
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tcsskipws(const tchar_t *s);

	/** Copy memory area.
	 * @param dest pointer to the copy destination
	 * @param src pointer to the source data
	 * @param n number of tchar_t characters to copy
	 * @return pointer to \a dest
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tmemcpy(void *dest, const void *src, size_t n);

	/** Copy memory area that can overlap.
	 * @param dest pointer to the copy destination
	 * @param src pointer to the source data
	 * @param n number of tchar_t characters to copy
	 * @return pointer to \a dest
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tmemmove(void *dest, const void *src, size_t n);

	/** Comare memory blocks.
	 * @param a pointer to memory block
	 * @param b pointer to memory block
	 * @param n number of tchar_t characters to compare
	 * @return an integer less than, equal to or greater than zero if \a a is found, respectively,
	 * to be less than, to match or be greater than \a b
	 */
	int pcl_tmemcmp(void *a, void *b, size_t n);
/** @} */
#elif defined(PCL_WINDOWS)
#	define PCL_TPATHSEP                   PCL_WPATHSEP
#	define PCL_TPATHSEPCHAR               PCL_WPATHSEPCHR
#	define pcl_tcslen                     wcslen
#	define pcl_tcschr                     wcschr
#	define pcl_tcsrchr                    wcsrchr
#	define pcl_totupper(c)                (wchar_t) towupper((wchar_t) (c))
#	define pcl_totlower(c)                (wchar_t) towlower((wchar_t) (c))
#	define pcl_tcsfbytes                  pcl_wcsfbytes
#	define pcl_tcscmp                     wcscmp
#	define pcl_tcsncmp                    wcsncmp
#	define pcl_tcsicmp                    pcl_wcsicmp
#	define pcl_tcsnicmp                   pcl_wcsnicmp
#	define pcl_tcsncpy                    pcl_wcsncpy
#	define pcl_tcsstr                     wcsstr
#	define pcl_tcsistr                    pcl_wcsistr
#	define pcl_tcsdup                     pcl_wcsdup
#	define pcl_tcsndup                    pcl_wcsndup
#	define pcl_tcsncat                    pcl_wcsncat
#	define pcl_tcscatf                    pcl_wcscatf
#	define pcl_vtcscatf                   pcl_vwcscatf
#	define pcl_atcscatf                   pcl_awcscatf
#	define pcl_vatcscatf                  pcl_vawcscatf
#	define pcl_tcstrim                    pcl_wcstrim
#	define pcl_tcsltrim                   pcl_wcsltrim
#	define pcl_tcsrtrim                   pcl_wcsrtrim
#	define pcl_tcstrimset                 pcl_wcstrimset
#	define pcl_tcsreplace                 pcl_wcsreplace
#	define pcl_tcsireplace                pcl_wcsireplace
#	define pcl_tcssplit                   pcl_wcssplit
#	define pcl_tcsisplit                  pcl_wcsisplit
#	define pcl_tcslower                   pcl_wcslower
#	define pcl_tcsupper                   pcl_wcsupper
#	define pcl_tcstrunc                   pcl_wcstrunc
#	define pcl_tcsslice                   pcl_wcsslice
#	define pcl_tcsrepchr                  pcl_wcsrepchr
#	define pcl_tcsskipws                  pcl_wcsskipws
#	define pcl_tmemcpy                    (tchar_t *) wmemcpy
#	define pcl_tmemmove                   (tchar_t *) wmemmove
#	define pcl_tmemcmp                    wmemcmp
#else
#	define PCL_TPATHSEP                   PCL_PATHSEP
#	define PCL_TPATHSEPCHAR               PCL_PATHSEPCHR
#	define pcl_tcslen                     strlen
#	define pcl_tcschr                     strchr
#	define pcl_tcsrchr                    strrchr
#	define pcl_totupper(c)                (char) toupper((int) (c))
#	define pcl_totlower(c)                (char) tolower((int) (c))
#	define pcl_tcsfbytes                  pcl_strfbytes
#	define pcl_tcscmp                     strcmp
#	define pcl_tcsncmp                    strncmp
#	define pcl_tcsicmp                    pcl_stricmp
#	define pcl_tcsnicmp                   pcl_strnicmp
#	define pcl_tcsncpy                    pcl_strncpy
#	define pcl_tcsstr                     strstr
#	define pcl_tcsistr                    pcl_stristr
#	define pcl_tcsdup                     pcl_strdup
#	define pcl_tcsndup                    pcl_strndup
#	define pcl_tcsncat                    pcl_strncat
#	define pcl_tcscatf                    pcl_strcatf
#	define pcl_vtcscatf                   pcl_vstrcatf
#	define pcl_atcscatf                   pcl_astrcatf
#	define pcl_vatcscatf                  pcl_vastrcatf
#	define pcl_tcstrim                    pcl_strtrim
#	define pcl_tcsltrim                   pcl_strltrim
#	define pcl_tcsrtrim                   pcl_strrtrim
#	define pcl_tcstrimset                 pcl_strtrimset
#	define pcl_tcsreplace                 pcl_strreplace
#	define pcl_tcsireplace                pcl_strireplace
#	define pcl_tcssplit                   pcl_strsplit
#	define pcl_tcsisplit                  pcl_strisplit
#	define pcl_tcslower                   pcl_wcslower
#	define pcl_tcsupper                   pcl_wcsupper
#	define pcl_tcstrunc                   pcl_strtrunc
#	define pcl_tcsslice                   pcl_strslice
#	define pcl_tcsrepchr(s, oldc, newc)   pcl_strrepchr(s, (int) (oldc), (int) (newc))
#	define pcl_tcsskipws                  pcl_strskipws
#	define pcl_tmemcpy                    (tchar_t *) memcpy
#	define pcl_tmemmove                   (tchar_t *) memmove
#	define pcl_tmemcmp                    memcmp
#endif

#ifndef __doxygen__
#	define pcl_tcscpy(dest, size, src) pcl_tcsncpy(dest,size,src,0)
#	define pcl_tcscat(dest, size, src) pcl_tcsncat(dest,size,src,0)
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif