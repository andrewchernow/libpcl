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

#ifndef LIBPCL_DEFS_H
#define LIBPCL_DEFS_H

#if defined(_M_X64) || \
    defined(_M_IA64) || \
    defined(__x86_64) || \
    defined(__x86_64__) || \
    defined(__alpha__) || \
    defined(__arch64__) || \
    defined(_ARCH_PPC64) || \
    defined(__sparcv9) || \
    defined(__amd64) || \
    defined(__amd64__) || \
    defined(_IA64) || \
    defined(__ia64__) || \
    defined(__IA64__) || \
    defined(_WIN64) || \
    __WORDSIZE == 64
	#define PCL_64BIT
#endif

#ifdef PCL_WINDOWS
	#pragma warning( disable : 4204 4710 4214 4706 4710 4711 4100 4820 )
	#undef WINVER
	#undef _WIN32_WINNT
	#undef _USE_32BIT_TIME_T
	#define _CRT_SECURE_NO_WARNINGS
	#define _CRT_SECURE_NO_DEPRECATE
	#define _CRT_NONSTDC_NO_WARNINGS
	#define WINVER 0x0A00 /* 0x0A00 is windows 10 */
	#define _WIN32_WINNT 0x0A00
	#define __func__ __FUNCTION__
	#define PCL_WPATHSEP L"\\"
	#define PCL_WPATHSEPCHR L'\\'
	#define PCL_PATHSEP "\\"
	#define PCL_PATHSEPCHR '\\'
	#undef alloca
	#define alloca _malloca
	#define pcl_ftell _ftelli64
#else
	#define PCL_WPATHSEP L"/"
	#define PCL_WPATHSEPCHR L'/'
	#define PCL_PATHSEP "/"
	#define PCL_PATHSEPCHR '/'
	#define pcl_ftell ftello
#endif

/** A PCL location is the file, func and line where a call occurred.
 * Whenever an PCL function/macro requires these 3 values, you can do:
 * @code
 * debug_func(PCL_LOCATION_ARGS)
 * debug_func(__FILE__, __func__, __LINE__) // instead of this
 * @endcode
 */
#define PCL_LOCATION_ARGS __FILE__, __func__, __LINE__

/** PCL_LOCATION_PARAMS expands to a function/macro declaration's parameter
 * list: ie. const char *file, etc.... Thus, declaring:
 * @code
 * void debug_func(const char *file, const char *func, int line)
 * void debug_func(PCL_LOCATION_PARAMS) // shortened to this
 * @endcode
 */
#define PCL_LOCATION_PARAMS const char *file, const char *func, int line

#define PCL_LOCATION_VALS file,func,line

/* usage: UNUSED(var1 || var2 || etc...) */
#define UNUSED(...) (void)(__VA_ARGS__)

#ifndef roundup
	#define roundup(val, mult) ((((val)+((mult)-1))/(mult))*(mult))
#endif

#ifndef max
	#ifdef MAX
		#define max(a,b) MAX(a,b)
	#else
		#define max(a, b) (((a) > (b)) ? (a) : (b))
	#endif
#endif
#ifndef min
	#ifdef MIN
		#define min(a,b) MIN(a,b)
	#else
		#define min(a, b) (((a) < (b)) ? (a) : (b))
	#endif
#endif

/* returns true if str is NULL or an empty string */
#define strempty(str) ((str) == NULL || !*(str))

/* get element count of an array (fixed array). Works strings (wide as well) */
#define countof(buffer) (int)(sizeof(buffer) / sizeof(buffer[0]))

/* zero a buffer */
#define zero(s) memset((char *)&(s), 0, sizeof(s))

/* Same as windows INVALID_HANDLE_VALUE or unix -1 fd. */
#define PCL_BADFD ((pcl_fd_t)(intptr_t)-1)

#endif // LIBPCL_DEFS_H
