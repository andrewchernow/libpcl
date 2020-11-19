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

#ifndef LIBPCL__ERRDEFS_H
#define LIBPCL__ERRDEFS_H

/** Saves the current OS error code, executes code_fragment and resets OS error code.
 * This is useful when performing cleanup code and it is desired to preserve the OS error code.
 * @param code_fragment one or more statments to execute
 * @see SAVE_ERRCTX
 */
#define SAVE_OSERR(code_fragment) do{ \
  uint32_t __e = pcl_oserrno; \
  { code_fragment; }; \
  pcl_setoserrno(__e); \
}while(0)

/** Sets the current thread's PCL error code.
 * @param _pclerr PCL error code
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETERR(_pclerr) SETERRMSG_OS(_pclerr, 0, NULL, 0)

/** Sets the current thread's PCL error code and OS error code.
 * Setting both PCL and OS error codes can be useful in situations where preserving the OS
 * error code provides more insight:
 * @code
 * // indicate badarg but preserve why it is bad (ENOENT, ERROR_FILE_NOT_FOUND, etc)
 * if(stat("does not exist", &st))
 *   SETERR_OS(PCL_EINVAL, pcl_oserrno);
 * @endcode
 * @param _pclerr PCL error code
 * @param _oserr OS error code
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETERR_OS(_pclerr, _oserr) SETERRMSG_OS(_pclerr, _oserr, NULL, 0)

/** Sets the current thread's PCL error code and error message
 * @param _pclerr PCL error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETERRMSG(_pclerr, _msgfmt, ...) SETERRMSG_OS(_pclerr, 0, _msgfmt, __VA_ARGS__)

/** Sets the current thread's PCL error code, OS error code and error message.
 * Setting both PCL and OS error codes can be useful in situations where preserving the OS
 * error code provides more insight:
 * @code
 * // indicate badarg but preserve why it is bad (ENOENT, ERROR_FILE_NOT_FOUND, etc)
 * if(stat("does not exist", &st))
 *   SETERRMSG_OS(PCL_EINVAL, pcl_oserrno, "missing conf file", 0);
 * @endcode
 * @param _pclerr PCL error code
 * @param _oserr OS error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETERRMSG_OS(_pclerr, _oserr, _msgfmt, ...)  \
  pcl_err_set(PCL_LOCATION_ARGS, _pclerr, _oserr, _msgfmt, __VA_ARGS__)

/** Sets the current thread's OS error code and maps code to PCL error code.
 * @param _oserr OS error code
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 * @see pcl_err_os2pcl
 */
#define SETOSERR(_oserr) SETOSERRMSG(_oserr, NULL, 0)

/** Sets the current thread's OS error code, error message and maps code to PCL error code.
 * @param _oserr OS error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETOSERRMSG(_oserr, _msgfmt, ...) \
  SETERRMSG_OS(pcl_err_os2pcl(_oserr), _oserr, _msgfmt, __VA_ARGS__)

/** Sets the current thread's last OS error code and maps code to PCL error code.
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETLASTERR() SETOSERRMSG(pcl_oserrno, NULL, 0)

/** Sets the current thread's last OS error code. error message and maps code to PCL error code.
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define SETLASTERRMSG(_msgfmt, ...) SETOSERRMSG(pcl_oserrno, _msgfmt, __VA_ARGS__)

/** Sets the return value and the current thread's PCL error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETERR(NULL, PCL_EINVAL); // returns NULL
 * return SETERR(PCL_EINVAL);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETERR(_retval, _pclerr) R_SETERRMSG_OS(_retval, _pclerr, 0, NULL, 0)

/** Sets the return value and the current thread's PCL error code and OS error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETERR_OS(NULL, PCL_EINVAL, ENOENT); // returns NULL
 * return SETERR_OS(PCL_EINVAL, ENOENT);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETERR_OS(_retval, _pclerr, _oserr) R_SETERRMSG_OS(_retval, _pclerr, _oserr, NULL, 0)

/** Sets the return value and the current thread's PCL error code and error message.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETERRMSG(NULL, PCL_EINVAL, "name cannot be NULL", 0); // returns NULL
 * return SETERRMSG(PCL_EINVAL, "name cannot be NULL", 0);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _pclerr PCL error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETERRMSG(_retval, _pclerr, _mgsfmt, ...) \
  R_SETERRMSG_OS(_retval, _pclerr, 0, _mgsfmt, __VA_ARGS__)

/** Sets the return value and the current thread's PCL error code, OS error code and error message.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETERRMSG_OS(NULL, PCL_EINVAL, ENOTDIR, "missing file", 0); // returns NULL
 * return SETERRMSG_OS(PCL_EINVAL, ENOTDIR, "missing file", 0);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _pclerr PCL error code
 * @param _oserr OS error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETERRMSG_OS(_retval, _pclerr, _oserr, _mgsfmt, ...) \
  (pcl_err_set(PCL_LOCATION_ARGS, _pclerr, _oserr, _mgsfmt, __VA_ARGS__),  (_retval))

/** Sets the return value and the current thread's OS error code and maps code to PCL error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETOSERR(NULL, EINVAL); // returns NULL
 * return SETOSERR(EINVAL);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _oserr OS error code
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETOSERR(_retval, _oserr) R_SETOSERRMSG(_retval, _oserr, NULL, 0)

/** Sets the return value and the current thread's OS error code, error message and maps code
 * to PCL error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETOSERRMSG(NULL, ENOENT, "missing '%s'", file); // returns NULL
 * return SETOSERRMSG(ENOENT, "missing '%s'", file);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _oserr OS error code
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETOSERRMSG(_retval, _oserr, _mgsfmt, ...) \
  R_SETERRMSG_OS(_retval, pcl_err_os2pcl(_oserr), _oserr, _mgsfmt, __VA_ARGS__)

/** Sets the return value and the current thread's last OS error code and maps code
 * to PCL error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETLASTERR(NULL); // returns NULL
 * return SETLASTERR();       // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETLASTERR(_retval) R_SETOSERRMSG(_retval, pcl_oserrno, NULL, 0)

/** Sets the return value and the current thread's last OS error code, error message and maps
 * code to PCL error code.
 * The return (R_) macros can be used to select a different return value (macro expansion value)
 * than the default of -1 or 0. This allows for:
 * @code
 * return R_SETLASTERRMSG(NULL, "problem", 0); // returns NULL
 * return SETLASTERRMSG("problem", 0);         // instead of this which returns -1
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return the value of _retval regardless of the PCL error code
 */
#define R_SETLASTERRMSG(_retval, _mgsfmt, ...) \
  R_SETOSERRMSG(_retval, pcl_oserrno, _mgsfmt, __VA_ARGS__)

/** Adds a stack trace to the current thread's error context.
 * When a nested function fails, known to have already set an error/trace, tracing macros can
 * be used to track the call stack.
 * @code
 * if(func_sets_pcl_error() < 0)
 *   return TRC();
 * @endcode
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define TRC() TRCMSG(NULL, 0)

/** Adds a stack trace to the current thread's error context with an error message.
 * When a nested function fails, known to have already set an error/trace, tracing macros can
 * be used to track the call stack.
 * @code
 * if(func_sets_pcl_error(value) < 0)
 *   return TRCMSG("bad value: %d", value);
 * @endcode
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return -1 if PCL error code is not PCL_OKAY(0), 0 otherwise
 */
#define TRCMSG(_msgfmt, ...) pcl_err_trace(PCL_LOCATION_ARGS, _msgfmt, __VA_ARGS__)

/** Sets the return value and adds a stack trace to the current thread's error context.
 * When a nested function fails, known to have already set an error/trace, tracing macros can
 * be used to track the call stack.
 * @code
 * if(func_sets_pcl_error() < 0)
 *   return R_TRC(NULL); // return NULL, not -1: ie TRC()
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @return the value of _retval regardless of the PCL error code
 */
#define R_TRC(_retval) R_TRCMSG(_retval, NULL, 0)

/** Sets the return value and adds a stack trace to the current thread's error context with
 * an error message.
 * When a nested function fails, known to have already set an error/trace, tracing macros can
 * be used to track the call stack.
 * @code
 * if(func_sets_pcl_error(value) < 0)
 *   return R_TRCMSG(NULL, "bad value: %d", value); // return NULL, not -1: ie TRCMSG("")
 * @endcode
 * @param _retval the value as the result of macro substitution.
 * @param _msgfmt error message as a printf-style format string
 * @param ... optional variable arguments for the printf-style _msgfmt
 * @return the value of _retval regardless of the PCL error code
 */
#define R_TRCMSG(_retval, _msgfmt, ...) \
  (pcl_err_trace(PCL_LOCATION_ARGS, _msgfmt, __VA_ARGS__),  (_retval))

/* wow is this used a lot, now it has a home. */
#define BADARG() SETERR(PCL_EINVAL)

#define PTRACE(_m, ...) do{ \
  TRC(); \
  pcl_err_fprintf(stderr, 0, _m, __VA_ARGS__); \
}while(0)

#define PANIC(_m, ...) do{ \
  PTRACE(_m,__VA_ARGS__); \
  exit(1); \
}while(0)

/* print a debug line with a formatted message */
#define PLNMSG(_m, ...) do{ \
  fprintf(stderr, "%s:%s(%d) - ", PCL_LOCATION_ARGS); \
  fprintf(stderr, _m, __VA_ARGS__);                   \
  fprintf(stderr, "\n");                          \
  fflush(stderr); \
}while(0)

/* print a debug line */
#define PLN do{ \
  fprintf(stderr, "%s:%s(%d)\n", PCL_LOCATION_ARGS); \
  fflush(stderr); \
}while(0)

#endif // LIBPCL__ERRDEFS_H
