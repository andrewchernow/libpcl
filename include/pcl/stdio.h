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

#ifndef LIBPCL_STDIO_H__
#define LIBPCL_STDIO_H__

#include <pcl/types.h>
#include <stdio.h>

/* these don't exist on windows. The numbers match windows whence values */
#ifndef SEEK_SET
	#define SEEK_SET 0 /* FILE_BEGIN: Seek from beginning of file */
#endif
#ifndef SEEK_CUR
	#define SEEK_CUR 1 /* FILE_CURRENT: Seek from current position */
#endif
#ifndef SEEK_END
	#define SEEK_END 2 /* FILE_END: Set file pointer to EOF plus "offset" */
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

/** Pipe a stream to or from a process. This differrs from the POSIX popen in that there is
 * no type argument. Instead, this using symbols (like Perl open) to indicate redirection
 * and optional flags as the first set of characters within the command arguments.
 *
 * @code
 * Mode     Description         Same as...
 *   &lt;      read from stdout    popen("cmd", "r")
 *   >      write to stdin      popen("cmd", "w")
 *   ^      read from stderr    [no equivalent, ^ used to mimic FISH shell]
 *
 * The below documents the other "flags" that can be specified in mode:
 *
 *   -      set FD_CLOEXEC      popen("cmd", "re" OR "we") <- since glibc 2.9
 *   !      set O_NONBLOCK      [no equivalent]
 *
 * // read from the process's stdout without blocking. Process created with
 * // the following command: `ls -l /home/username`
 * FILE *stream = pcl_popen(T("&lt;! ls -l %s"), getenv("HOME"));
 * @endcode
 *
 * @param command
 * @param ...
 * @return
 */
PCL_EXPORT FILE *pcl_popen(const tchar_t *command, ...);

PCL_EXPORT int pcl_fileno(FILE *stream);

/* Opens a pcl_file_t as a FILE stream.  On success, a FILE stream is returned that must be closed
 * via fclose.  The provided pcl_file_t is released and cannot be used again.  On error, NULL is
 * returned and the provided pcl_file_t is left unchanged and completely usable.
 *
 * FILE *fp = pcl_fdopen(file);
 *
 * // success, 'file' is no longer valid ... do not use
 *
 * if(fp) // success
 * {
 *   fprintf(fp, "'fp' is valid here! 'file' is not.\n");
 *   fclose(fp);
 * }
 * else   // failure
 * {
 *   pcl_file_printf(file, "'file' is valid here.  'fp' is not.\n");
 *   pcl_close(file); // callee still responsible for this...
 * }
 */
PCL_EXPORT FILE *pcl_fdopen(pcl_file_t *file);

/** Change the name or location of a file
 * @param oldname The old name/path of the file
 * @param newname if this exists, it will be replaced
 * @return 0 on success and -1 on error
 */
PCL_EXPORT int pcl_rename(const tchar_t *oldname, const tchar_t *newname);

/** Remove a file or directory. The directory must be empty. File can be a link, socket, FIFO
 * or device.
 * @param pathname
 * @return 0 on success or -1 on error
 */
PCL_EXPORT int pcl_remove(const tchar_t *pathname);

PCL_EXPORT int pcl_unlink(const tchar_t *path);

/** Write a formatted character string to stdout.
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_printf(const char *format, ...);

/** Write a formatted wide-character string to stdout.
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_wprintf(const wchar_t *format, ...);

/** Write a formatted character string to stdout.
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_vprintf(const char *format, va_list ap);

/** Write a formatted wide-character string to stdout.
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the stdout or a negative PCL error code.
 */
PCL_EXPORT int pcl_vwprintf(const wchar_t *format, va_list ap);

/** Write a formatted character string to the given stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_fprintf(FILE *stream, const char *format, ...);

/** Write a formatted wide-character string to the given stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_fwprintf(FILE *stream, const wchar_t *format, ...);

/** Write a formatted character string to the given stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_vfprintf(FILE *stream, const char *format, va_list ap);

/** Write a formatted wide-character string to the given stream.
 * @param stream file stream to write to
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the stream or a negative PCL error code.
 */
PCL_EXPORT int pcl_vfwprintf(FILE *stream, const wchar_t *format, va_list ap);

/** Write a formatted character string to the given buffer.
 * @param buf a buffer os size characters
 * @param size size in characters of buf argument
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the buffer or if buf is null, the number of
 * characters that would have been written to buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_sprintf(char *buf, size_t size, const char *format, ...);

/** Write a formatted wide-character string to the given buffer.
 * @param buf a buffer os size characters
 * @param size size in characters of buf argument
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the buffer or if buf is null, the number of
 * characters that would have been written to buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_swprintf(wchar_t *buf, size_t size, const wchar_t *format, ...);

/** Write a formatted character string to an allocated buffer.
 * @param out output pointer, which requires a free when no longer needed
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the buffer or if out is null, the number of
 * characters that would have been written to out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_asprintf(char **out, const char *format, ...);

/** Write a formatted wide-character string to an allocated buffer.
 * @param out output pointer, which requires a free when no longer needed
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ... variable arguments matching format
 * @return number of characters written to the buffer or if out is null, the number of
 * characters that would have been written to out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_aswprintf(wchar_t **out, const wchar_t *format, ...);

/** Write a formatted character string to the given buffer.
 * @param buf a buffer os size characters
 * @param size size in characters of buf argument
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the buffer or if buf is null, the number of
 * characters that would have been written to buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vsprintf(char *buf, size_t size, const char *format, va_list ap);

/** Write a formatted wide-character string to the given buffer.
 * @param buf a buffer os size characters
 * @param size size in characters of buf argument
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the buffer or if buf is null, the number of
 * characters that would have been written to buf. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vswprintf(wchar_t *buf, size_t size, const wchar_t *format, va_list ap);

/** Write a formatted character string to an allocated buffer.
 * @param out output pointer, which requires a free when no longer needed
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the buffer or if out is null, the number of
 * characters that would have been written to out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vasprintf(char **out, const char *format, va_list ap);

/** Write a formatted wide-character string to an allocated buffer.
 * @param out output pointer, which requires a free when no longer needed
 * @param format format specifier string. PCL includes extensions: "%/" is the platform-specific
 * path separater character, "%ts" "%Ts" is a tchar_t string, "%tc" "%Tc" is a tchar_t.
 * @param ap variable argument list
 * @return number of characters written to the buffer or if out is null, the number of
 * characters that would have been written to out. On error, a negative PCL error code is
 * returned.
 */
PCL_EXPORT int pcl_vaswprintf(wchar_t **out, const wchar_t *format, va_list ap);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_STDIO_H__
