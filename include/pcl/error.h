
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

#ifndef LIBPCL_ERROR_H
#define LIBPCL_ERROR_H

#include <pcl/types.h>
#include <pcl/defs.h>
#include <pcl/_errno.h>
#include <pcl/_errdefs.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Gets the calling thread's error context
 * @return pointer to the thread's context
 */
PCL_EXPORT pcl_err_ctx_t *pcl_err_ctx(void);


/* -------------------------------------------------------------------------
 * Last Error API
 */

/** Gets the last PCL error, mimics 'errno'. */
#define pcl_errno pcl_err_last()

#ifdef PCL_WINDOWS
#	define pcl_oserrno pcl_win32_oserrno()
	PCL_EXPORT uint32_t pcl_win32_oserrno(void);
#	define pcl_crterrno errno
#	define pcl_sockerrno WSAGetLastError()
#else
#	define pcl_oserrno errno
#	define pcl_crterrno errno
#	define pcl_sockerrno errno
#endif

/** Gets the last PCL err code. This is typically accessed through the pcl_errno macro.
 * @return last PCL error code
 */
PCL_EXPORT int pcl_err_last(void);

/** Get the last error message, stack trace tail.
 * @return pointer to the last error message
 */
PCL_EXPORT char *pcl_err_lastmsg(void);


/* -------------------------------------------------------------------------
 * Formatted Output API
 */

/* Print error info and full stack trace to the given stream */
PCL_EXPORT int pcl_err_fprintf(FILE *stream, int indent, const char *format, ...);

/* va_list version of pcl_err_fprintf */
PCL_EXPORT int pcl_err_vfprintf(FILE *stream, int indent, const char *format, va_list args);

/* Print error info and full stack trace to the given buffer */
PCL_EXPORT int pcl_err_sprintf(char *buf, size_t buf_size, int indent, const char *format, ...);

/* va_list version of pcl_err_sprintf */
PCL_EXPORT int pcl_err_vsprintf(char *buf, size_t buf_size, int indent, const char *format,
	va_list args);


/* -------------------------------------------------------------------------
 * Error Set API
 */

#ifdef PCL_WINDOWS
#	define pcl_setoserrno(e) pcl_win32_setoserrno(e)
	PCL_EXPORT uint32_t pcl_win32_setoserrno(uint32_t e);
#	define pcl_setcrterrno(e) (_set_errno(e), (e))
#else
#	define pcl_setoserrno(e) errno = (int)(e)
#	define pcl_setcrterrno(e) pcl_setoserrno(e)
#endif

PCL_EXPORT int pcl_err_clear(void);

#define pcl_err_osclear() (pcl_setoserrno(0), pcl_setcrterrno(0))

/* va_list version of pcl_err_set */
PCL_EXPORT int pcl_err_vset(PCL_LOCATION_PARAMS, int err, uint32_t oserr,
	const char *format, va_list ap);

/* Sets the PCL error, adds a trace and message.  Returns -1 when error
 * is not PCL_EOKAY.  Normally not called directly, see SETERRxxx macros.
 */
PCL_EXPORT int pcl_err_set(PCL_LOCATION_PARAMS, int err, uint32_t oserr, const char *format, ...);


/* -------------------------------------------------------------------------
 * Stack Trace API
 */

/* Adds a trace and a message to the error context's stack trace.
 * Returns -1 when the context's error is not PCL_EOKAY. See TRC* and R_TRC* macros.
 */
PCL_EXPORT int pcl_err_trace(PCL_LOCATION_PARAMS, const char *format, ...);

/* va_list version of pcl_err_trace */
PCL_EXPORT int pcl_err_vtrace(PCL_LOCATION_PARAMS, const char *format, va_list ap);


/* -------------------------------------------------------------------------
 * Error Mapping functions
 */

/* Get the default PCL error message given an PCL error code. */
PCL_EXPORT const char *pcl_err_msg(int err);

/* Get the PCL error name given an PCL error code. */
PCL_EXPORT const char *pcl_err_name(int err);

/* Get the PCL error code given an PCL error name. */
PCL_EXPORT int pcl_err_code(const char *pclname);

/* Get the PCL error code given an OS error code. */
PCL_EXPORT int pcl_err_os2pcl(uint32_t oserr);

/* Get the OS error name given an OS error code. */
PCL_EXPORT const char *pcl_err_osname(uint32_t oserr);

/* Get the default OS error message given an OS error code (like strerror).
 * Returns the number of chars written to the osmsg buffer. If oserr is 0,
 * this returns 0 and terminates buf at index 0.
 */
PCL_EXPORT int pcl_err_osmsg(uint32_t oserr, char *osmsg, size_t size);

#ifdef PCL_WINDOWS
	#define pcl_err_crt2pcl(_crt_errno) pcl_err_os2pcl(pcl_err_crt2os(_crt_errno))
PCL_EXPORT unsigned long pcl_err_crt2os(int err);
#else
	#define pcl_err_crt2pcl(_crt_errno) pcl_err_os2pcl(_crt_errno)
	#define pcl_err_crt2os(_crt_errno) _crt_errno
#endif

#ifdef __cplusplus
}
#endif
#endif
