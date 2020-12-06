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

#ifndef LIBPCL_STRINT_H
#define LIBPCL_STRINT_H

/** @defgroup strint String to Integer Conversion
 * Convert strings to integers.
 * @{
 */
#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Convert a character string to an integer.
* The integer conversion functions do range checks in addition to the initial conversion. If
* the converted value is out of range, -1 is returned and \c PCL_ERANGE is set.
*
* @param nptr pointer to a character string
* @param end a pointer that if provided, will point to the first invalid character
* @param base integer base between 2 and 36 inclusive or special value of 0. When 0,
* if \a nptr begins with '0x', base 16 is used. If \a nptr begins with '0', octal is used.
* Otherwise, base 10 is used.
* @param r pointer to the result of the conversion
* @return 0 on success and -1 on error.
*/
PCL_EXPORT int pcl_strtoi(const char *nptr, char **end, int base, int *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstoi(const wchar_t *nptr, wchar_t **end, int base, int *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_strtoui(const char *nptr, char **end, int base, unsigned int *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstoui(const wchar_t *nptr, wchar_t **end, int base, unsigned int *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_strtol(const char *nptr, char **end, int base, long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstol(const wchar_t *nptr, wchar_t **end, int base, long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_strtoul(const char *nptr, char **end, int base, unsigned long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstoul(const wchar_t *nptr, wchar_t **end, int base, unsigned long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_strtoll(const char *nptr, char **end, int base, long long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstoll(const wchar_t *nptr, wchar_t **end, int base, long long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_strtoull(const char *nptr, char **end, int base, unsigned long long *r);
/**  @copydoc pcl_strtoi */
PCL_EXPORT int pcl_wcstoull(const wchar_t *nptr, wchar_t **end, int base, unsigned long long *r);

#ifdef __doxygen__
/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstoi(const pchar_t *nptr, pchar_t **end, int base, int *r);

	/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstoui(const pchar_t *nptr, pchar_t **end, int base, unsigned int *r);

	/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstol(const pchar_t *nptr, pchar_t **end, int base, long *r);

	/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstoul(const pchar_t *nptr, pchar_t **end, int base, unsigned long *r);

	/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstoll(const pchar_t *nptr, pchar_t **end, int base, long long *r);

	/** @copydoc pcl_strtoi
	 * @note implemented as a macro
	 */
	int pcl_pcstoull(const pchar_t *nptr, pchar_t **end, int base, unsigned long long *r);
#elif defined(PCL_WINDOWS)
#	define pcl_pcstoi                     pcl_wcstoi
#	define pcl_pcstoui                    pcl_wcstoui
#	define pcl_pcstol                     pcl_wcstol
#	define pcl_pcstoul                    pcl_wcstoul
#	define pcl_pcstoll                    pcl_wcstoll
#	define pcl_pcstoull                   pcl_wcstoull
#else
#	define pcl_pcstoi                     pcl_strtoi
#	define pcl_pcstoui                    pcl_strtoui
#	define pcl_pcstol                     pcl_strtol
#	define pcl_pcstoul                    pcl_strtoul
#	define pcl_pcstoll                    pcl_strtoll
#	define pcl_pcstoull                   pcl_strtoull
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_STRINT_H
