
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

#ifndef LIBPCL_LOG_H
#define LIBPCL_LOG_H

#include <pcl/time.h>
#include <pcl/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/* log severity levels */
#define PCL_LOGLEV_DEBUG 0x000001u
#define PCL_LOGLEV_INFO  0x000002u
#define PCL_LOGLEV_WARN  0x000004u
#define PCL_LOGLEV_ERROR 0x000008u
#define PCL_LOGLEV_FATAL 0x000010u
#define PCL_LOGLEV_PANIC 0x000020u

/* trace bits for each level */
#define PCL_LOGTRC_DEBUG 0x000100u
#define PCL_LOGTRC_INFO  0x000200u
#define PCL_LOGTRC_WARN  0x000400u
#define PCL_LOGTRC_ERROR 0x000800u
#define PCL_LOGTRC_FATAL 0x001000u
#define PCL_LOGTRC_PANIC 0x002000u

/* display UTC time (defaults to localtime). bitwise OR in with
 * pcl_log_event_t.level.
 */
#define PCL_LOGOPT_UTC 0x010000u

#define PCL_LOG_MAXLEVELSTR 48u
#define PCL_LOGLEV_MASK  0x00ffu /* log level mask */
#define PCL_LOGTRC_MASK  0xff00u /* log trace mask */
#define PCL_LOGTRC_LSH   8u    /* no. of bits to left shift level to get trace bit */

/* Convert a _level bit to a trace bit */
#define PCL_LOGLEV_TOTRACE(_level) \
  (((_level) & (unsigned)PCL_LOGLEV_MASK) << (unsigned)PCL_LOGTRC_LSH)

/* Test if a _level bit has it's trace bit set (enabled/on) */
#define PCL_LOGLEV_HASTRACE(_level) ((_level) & PCL_LOGLEV_TOTRACE(_level))

/* time format used by the PCL logging system. classic syslog/rsyslog format
 * This can be redefined and set to any valid `strftime(3)` format. The
 * resulting format is limited to 128 characters.
 */
#ifndef PCL_LOG_TIMEFORMAT
#	define PCL_LOG_TIMEFORMAT "%b %d %H:%M:%S"
#endif

/* suggested fopen mode to use when opening a PCL log file */
#define PCL_LOG_OPEN_MODE _T("a+b")

/* This structure represents a PCL log event. It must be filled out and passed
 * to either call pcl_log_event or pcl_log_vevent.
 */
typedef struct tag_pcl_log_event pcl_log_event_t;

struct tag_pcl_log_event
{
	FILE *stream;          /* application stream (standard log file), can be NULL */

	/* EVENT LOCATION */
	const char *file;      /* file where log event was created */
	const char *func;      /* function where log event was created */
	int line;              /* line number where log event was created */

	/* EVENT DETAILS */
	pcl_time_t when;       /* when the event occurred */
	uint32_t level;        /* log level OR'd with possible log trace bit */
	const char *facility;  /* name for the app/thread/etc... that is logging the event, can be NULL */
	pcl_err_ctx_t *ctx;    /* optional context, if NULL the current thread's context is used. */

	/* EXIT CONFIG: like panic or even fatal */
	int exit_code;         /* exit code to use by exit_handler or by logging system if
	                          exit_handler is NULL or doesn't actually exit. */
	void *exit_context;    /* user context pointer passed to optional 'exit_handler'. */
	int exit_levels;       /* bit mask of levels that signify an app exit condition,
	                          where the PCL logging system calls exit_handler or exit(exit_code) if
	                          no exit_handler is present and exit_levels is non-zero.
	                          If this value is zero, the PCL logging will never exit
	                          on it's own or call the exit_handler. */
	void (*exit_handler)(  /* If exit level is detected, use this callback to exit, */
		pcl_log_event_t *e,     /* otherwise PCL logging system will call exit(exit_code)) if */
		void *context);         /* exit_levels match the current event level. */
};

/* Placeholder PCL_LOG_EVENT macro implementation function. Any production app
 * should redefine PCL_LOG_EVENT macro to call an app-specific function.
 */
PCL_EXPORT int
pcl_log_event_default(PCL_LOCATION_PARAMS, uint32_t level, const char *message, ...);

/* Returns the number of bytes written to all possible streams. For a
 * breakdown of bytes written to each possible stream, see the
 * pcl_log_event_t structure. On error, -1 is returned.
 */
PCL_EXPORT int
pcl_log_event(pcl_log_event_t *e, const char *message, ...);

/* Returns the number of bytes written to all possible streams. For a
 * breakdown of bytes written to each possible stream, see the
 * pcl_log_event_t structure. On error, -1 is returned.
 */
PCL_EXPORT int
pcl_log_vevent(pcl_log_event_t *e, const char *message, va_list ap);

/* LOG_EVENT macros, can be disabled via PCL_NO_LOG_EVENT_MACROS */
#ifndef PCL_NO_LOG_EVENT_MACROS

/* Log an event. This macro is designed to be undef'd and redefined to an app-specific
 * log event function. This is really just a placeholder. See pcl/src/log.c
 * for an example of implementing the PCL_LOG_EVENT macro function properly.
 */
#define PCL_LOG_EVENT(level, msg, ...) \
  pcl_log_event_default(PCL_LOCATION_ARGS, (uint32_t)(level), msg, __VA_ARGS__)

#define LOG_PANIC(msg, ...)       PCL_LOG_EVENT(PCL_LOGLEV_PANIC, msg, __VA_ARGS__)
#define LOG_FATAL(msg, ...)       PCL_LOG_EVENT(PCL_LOGLEV_FATAL, msg, __VA_ARGS__)
#define LOG_ERROR(msg, ...)       PCL_LOG_EVENT(PCL_LOGLEV_ERROR, msg, __VA_ARGS__)
#define LOG_WARN(msg, ...)        PCL_LOG_EVENT(PCL_LOGLEV_WARN, msg, __VA_ARGS__)
#define LOG_INFO(msg, ...)        PCL_LOG_EVENT(PCL_LOGLEV_INFO, msg, __VA_ARGS__)
#define LOG_DEBUG(msg, ...)       PCL_LOG_EVENT(PCL_LOGLEV_DEBUG, msg, __VA_ARGS__)

/* Set an PCL error and log an event */
#define PCL_LOG_ERR(_e, _l, _m, ...) do{ \
  SETERR(_e); \
  PCL_LOG_EVENT(_l,_m,__VA_ARGS__); \
}while(0)
#define LOG_PANIC_ERR(_e, _m, ...)  PCL_LOG_ERR(_e,PCL_LOGLEV_PANIC,_m,__VA_ARGS__)
#define LOG_FATAL_ERR(_e, _m, ...)  PCL_LOG_ERR(_e,PCL_LOGLEV_FATAL,_m,__VA_ARGS__)
#define LOG_ERROR_ERR(_e, _m, ...)  PCL_LOG_ERR(_e,PCL_LOGLEV_ERROR,_m,__VA_ARGS__)
#define LOG_WARN_ERR(_e, _m, ...)   PCL_LOG_ERR(_e,PCL_LOGLEV_WARN,_m,__VA_ARGS__)
#define LOG_INFO_ERR(_e, _m, ...)   PCL_LOG_ERR(_e,PCL_LOGLEV_INFO,_m,__VA_ARGS__)
#define LOG_DEBUG_ERR(_e, _m, ...)  PCL_LOG_ERR(_e,PCL_LOGLEV_DEBUG,_m,__VA_ARGS__)

/* Set an OS error and log an event */
#define PCL_LOG_OS(_o, _l, _m, ...) do{ \
  SETOSERR(_o); \
  PCL_LOG_EVENT(_l,_m,__VAR_ARGS__); \
}while(0)
#define LOG_PANIC_OS(_o, _m, ...)   PCL_LOG_OS(_o,PCL_LOGLEV_PANIC,_m,__VA_ARGS__)
#define LOG_FATAL_OS(_o, _m, ...)   PCL_LOG_OS(_o,PCL_LOGLEV_FATAL,_m,__VA_ARGS__)
#define LOG_ERROR_OS(_o, _m, ...)   PCL_LOG_OS(_o,PCL_LOGLEV_ERROR,_m,__VA_ARGS__)
#define LOG_WARN_OS(_o, _m, ...)    PCL_LOG_OS(_o,PCL_LOGLEV_WARN,_m,__VA_ARGS__)
#define LOG_INFO_OS(_o, _m, ...)    PCL_LOG_OS(_o,PCL_LOGLEV_INFO,_m,__VA_ARGS__)
#define LOG_DEBUG_OS(_o, _m, ...)   PCL_LOG_OS(_o,PCL_LOGLEV_DEBUG,_m,__VA_ARGS__)

/* Set the last OS error and log an event */
#define PCL_LOG_LAST(_l, _m, ...) do{ \
  SETLASTERR(); \
  PCL_LOG_EVENT(_l,_m,__VA_ARGS__); \
}while(0)
#define LOG_PANIC_LAST(_m, ...)    PCL_LOG_LAST(PCL_LOGLEV_PANIC,_m,__VA_ARGS__)
#define LOG_FATAL_LAST(_m, ...)    PCL_LOG_LAST(PCL_LOGLEV_FATAL,_m,__VA_ARGS__)
#define LOG_ERROR_LAST(_m, ...)    PCL_LOG_LAST(PCL_LOGLEV_ERROR,_m,__VA_ARGS__)
#define LOG_WARN_LAST(_m, ...)     PCL_LOG_LAST(PCL_LOGLEV_WARN,_m,__VA_ARGS__)
#define LOG_INFO_LAST(_m, ...)     PCL_LOG_LAST(PCL_LOGLEV_INFO,_m,__VA_ARGS__)
#define LOG_DEBUG_LAST(_m, ...)    PCL_LOG_LAST(PCL_LOGLEV_DEBUG,_m,__VA_ARGS__)

#endif /* !PCL_NO_LOG_EVENT_MACROS */

/** convert an PCL_LOGLEV_xxx bit value to a string
 * @param[in] level log level
 * @return the level name as a string or ""
 */
PCL_INLINE const char *
pcl_log_strlevel(uint32_t level)
{
	switch(level & PCL_LOGLEV_MASK)
	{
		case PCL_LOGLEV_PANIC:
			return "panic";
		case PCL_LOGLEV_FATAL:
			return "fatal";
		case PCL_LOGLEV_ERROR:
			return "error";
		case PCL_LOGLEV_WARN:
			return "warn";
		case PCL_LOGLEV_INFO:
			return "info";
		case PCL_LOGLEV_DEBUG:
			return "debug";
		default:
			return "";
	}
}

#ifdef __cplusplus
}
#endif

#endif /* !LIBPCL_LOG_H */
