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

#ifndef LIBPCL_IO_H__
#define LIBPCL_IO_H__

/** @defgroup io Input/Output Functions
 * The \c IO module defines some basic IO functions as well as a suite of printf-style
 * formatting functions.
 * @{
 */

#include <pcl/types.h>

// these exist on windows but under different names
#ifdef PCL_WINDOWS
#	define SEEK_SET 0 // FILE_BEGIN
#	define SEEK_CUR 1 // FILE_CURRENT
#	define SEEK_END 2 // FILE_END
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** Opens a FILE stream for reading and/or writing.
 * @param path absolute or relative path
 * @param mode Can be 'r', 'r+', 'w', 'w+', 'a', 'a+', see any unix man page. Supports 'b'.
 * @return a valid stream or NULL on error
 */
PCL_EXPORT FILE *pcl_fopen(const tchar_t *path, const tchar_t *mode);

/** Pipe a stream to or from a process. This differs from the POSIX popen in that there is
 * no type argument. Instead, this using symbols (like Perl open) to indicate redirection
 * and optional flags as the first set of characters within the command arguments.
 *
 * @code
 * Mode     Description         Same as...
 *   <      read from stdout    popen("cmd", "r")
 *   >      write to stdin      popen("cmd", "w")
 *   ^      read from stderr    [no equivalent, ^ used to mimic FISH shell]
 *
 * The below documents the other "flags" that can be specified in mode:
 *
 *   !      set O_NONBLOCK      [no equivalent]
 *
 *
 * // read from the process's stdout without blocking. Process created with
 * // the following command: `ls -l /home/username`
 * tchar_t *homedir = pcl_getenv("HOME");
 * FILE *stream = pcl_popen(_T("<! ls -l %ts"), homedir);
 * pcl_free(homedir);
 * @endcode
 *
 * @param command pointer to the command and optional flags
 * @param ... variable arguments
 * @return stream pointer or \c NULL on error
 */
PCL_EXPORT FILE *pcl_popen(const tchar_t *command, ...);

/** Get a file descriptor from a stream
 * @param stream pointer to a \c FILE stream
 * @return file descriptor or -1 on error
 */
PCL_EXPORT int pcl_fileno(FILE *stream);

/** Open a file handle as a stream.
 * On success, a file stream is returned that must be closed via \c fclose.  The provided file
 * handle is released and cannot be used again. However, if this function fails, the caller
 * is still repsonsible for closing the given file handle.
 * @code
 * pcl_file_t *file = get_the_file();
 * FILE *fp = pcl_fdopen(file);
 *
 * if(fp) // success
 * {
 *   fprintf(fp, "'fp' is valid here! 'file' is not.\n");
 *   fclose(fp);
 * }
 * else // failure
 * {
 *   printf("'file' is valid here. 'fp' is not.\n");
 *   pcl_file_close(file); // callee still responsible for this...
 * }
 * @endcode
 * @param file pointer to a file handle
 * @return a file stream that must be closed with \c fclose
 */
PCL_EXPORT FILE *pcl_fdopen(pcl_file_t *file);

/** Create and open a unique temporary file.
 * This function will make several attempts to create the unique temporary file. The below is
 * a list (in order) of what directories are attempted:
 *  1. try the \a dir argument
 *  2. Windows \c TMP or Unix \c TMPDIR
 *  3. Windows \c TEMP or Unix \c P_tmpdir
 *  4. "."
 * @param dir Optional directory path to create temp file within, can be \c NULL.
 * @param prefix Optional prefix for the generated pathname. Can be \c NULL
 * @param suffix Optional suffix for the generated pathname. Can be \c NULL
 * @param pathp pointer to a ::tchar_t string that will receive the temp pathname. This
 * can be \c NULL. When not \c NULL, this must be freed by the caller.
 * @param keep If false, returned temp file will be automatically deleted once closed.
 * When true, the file will persist after file close.
 * @return
 */
PCL_EXPORT FILE *pcl_tmpfile(const tchar_t *dir, const tchar_t *prefix,
	const tchar_t *suffix, tchar_t **pathp, bool keep);

/** Write a formatted character string to \c stdout.
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to \c stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_printf(const char *format, ...);
/** @copydoc pcl_printf */
PCL_EXPORT int pcl_wprintf(const wchar_t *format, ...);

/** Write a formatted character string to \c stdout.
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to \c stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_vprintf(const char *format, va_list ap);
/** @copydoc pcl_vprintf */
PCL_EXPORT int pcl_vwprintf(const wchar_t *format, va_list ap);

/** Write a formatted character string to a stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_fprintf(FILE *stream, const char *format, ...);
/** @copydoc pcl_fprintf */
PCL_EXPORT int pcl_fwprintf(FILE *stream, const wchar_t *format, ...);

/** Write a formatted character string to the given stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_vfprintf(FILE *stream, const char *format, va_list ap);
/** @copydoc pcl_vfprintf */
PCL_EXPORT int pcl_vfwprintf(FILE *stream, const wchar_t *format, va_list ap);

/** Write a formatted character string to a buffer.
 * @param buf pointer to a buffer, set to \c NULL to query output length (excluding NUL)
 * @param size character length of \a buf, ignored if \a buf is \c NULL
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to \a buf or if \a buf is \c NULL, the number of
 * characters that would have been written to \a buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_sprintf(char *buf, size_t size, const char *format, ...);
/** @copydoc pcl_sprintf */
PCL_EXPORT int pcl_swprintf(wchar_t *buf, size_t size, const wchar_t *format, ...);

/** Write a formatted character string to a buffer.
 * @param buf pointer to a buffer, set to \c NULL to query output length (excluding NUL)
 * @param size character length of \a buf, ignored if \a buf is \c NULL
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to \a buf or if buf is \c NULL, the number of
 * characters that would have been written to \a buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vsprintf(char *buf, size_t size, const char *format, va_list ap);
/** @copydoc pcl_vsprintf */
PCL_EXPORT int pcl_vswprintf(wchar_t *buf, size_t size, const wchar_t *format, va_list ap);

/** Write a formatted character string to an allocated buffer.
 * @param[out] out pointer to an output pointer, this must be freed by caller. set to \c NULL
 * to query output length (excluding NUL)
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to \a out or if \a out is \c NULL, the number of
 * characters that would have been written to \a out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_asprintf(char **out, const char *format, ...);
/** @copydoc pcl_asprintf */
PCL_EXPORT int pcl_aswprintf(wchar_t **out, const wchar_t *format, ...);

/** Write a formatted character string to an allocated buffer.
 * @param[out] out pointer to an output pointer, this must be freed by caller. set to \c NULL to
 * query output length (excluding NUL)
 * @param format format specifier string. PCL includes extensions: \c "%/" is the platform-specific
 * path separater character, \c "%ts" \c "%Ts" is a tchar_t string, \c "%tc" \c "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to \a out or if \a out is \c NULL, the number of
 * characters that would have been written to \a out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vasprintf(char **out, const char *format, va_list ap);
/** @copydoc pcl_vasprintf */
PCL_EXPORT int pcl_vaswprintf(wchar_t **out, const wchar_t *format, va_list ap);

#ifdef __doxygen__
	/** @copydoc pcl_printf
	 * @note implemented as a macro
	 */
	int pcl_tprintf(const tchar_t *format, ...);

	/** @copydoc pcl_vprintf
	 * @note implemented as a macro
	 */
	int pcl_vtprintf(const tchar_t *format, va_list ap);

	/** @copydoc pcl_fprintf
	 * @note implemented as a macro
	 */
	int pcl_ftprintf(FILE *stream, const tchar_t *format, ...);

	/** @copydoc pcl_vfprintf
	 * @note implemented as a macro
	 */
	int pcl_vftprintf(FILE *stream, const tchar_t *format, va_list ap);

	/** @copydoc pcl_sprintf
	 * @note implemented as a macro
	 */
	int pcl_stprintf(tchar_t *buf, size_t size, const tchar_t *format, ...);

	/** @copydoc pcl_vsprintf
	 * @note implemented as a macro
	 */
	int pcl_vstprintf(tchar_t *buf, size_t size, const tchar_t *format, va_list ap);

	/** @copydoc pcl_asprintf
	 * @note implemented as a macro
	 */
	int pcl_astprintf(tchar_t **out, const tchar_t *format, ...);

	/** @copydoc pcl_vasprintf
	 * @note implemented as a macro
	 */
	int pcl_vastprintf(tchar_t **out, const tchar_t *format, va_list ap);
#elif defined(PCL_WINDOWS)
#	define pcl_tprintf                    pcl_wprintf
#	define pcl_vtprintf                   pcl_vwprintf
#	define pcl_ftprintf                   pcl_fwprintf
#	define pcl_vftprintf                  pcl_vfwprintf
#	define pcl_stprintf                   pcl_swprintf
#	define pcl_vstprintf                  pcl_vswprintf
#	define pcl_astprintf                  pcl_aswprintf
#	define pcl_vastprintf                 pcl_vaswprintf
#else
#	define pcl_tprintf                    pcl_printf
#	define pcl_vtprintf                   pcl_vprintf
#	define pcl_ftprintf                   pcl_fprintf
#	define pcl_vftprintf                  pcl_vfprintf
#	define pcl_stprintf                   pcl_sprintf
#	define pcl_vstprintf                  pcl_vsprintf
#	define pcl_astprintf                  pcl_asprintf
#	define pcl_vastprintf                 pcl_vasprintf
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif //LIBPCL_IO_H__
