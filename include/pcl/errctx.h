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

#ifndef LIBPCL_ERRCTX_H
#define LIBPCL_ERRCTX_H

/** @defgroup errctx Error Context Management
 * Provides functions for managing error contexts.
 * @{
 */
#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Stack trace object implemented as a linked list. */
struct tag_pcl_err_trace
{
	/** next trace in stack trace or NULL to indicate the end. */
	pcl_err_trace_t *next;
	char *file;
	char *func;
	int line;

	/** optional user-defined message */
	char *msg;

	/** allocation size of trace structure. The trace object is a single allocation and
	 * all string members are self-relative pointers
	 */
	size_t size;
};

/** Represents a PCL error context, with optional stack trace. This can be thought of
 * as Unix `errno` on steroids.
 */
struct tag_pcl_err_ctx
{
	/** Freezes context, making it immutable. It should be noted that a frozen context object
	 * cannot be cleared or freed. The only exception is the per-thread context, which can and
	 * will be freed if the thread exits.
	 *
	 * Typically, freezing an object is only useful when used with the PCL error API. For example:
	 * freezing the per-thread context before performing cleanup after an error, where the
	 * cleanup could overwrite the "real" error: ie. `freeze=true; cleanup_code; freeze=false;`.
	 *
	 * Internally, all printf-style context functions freeze the object before performing output
	 * and thaw before returning.
	 *
	 * @see FREEZE_ERR
	 */
	bool frozen;

	/** PCL error code: PCL_EXXX */
	int err;

	/** Platform-specific error code: Unix errno, Windows Error (ie: GetLastError, _dos_errno),
	 * etc. This can be zero even when 'err' is not. However, if this is non-zero 'err' will
	 * also be non-zero.
	 */
	uint32_t oserr;

	/** Stack trace implemented as a linked list. This is NULL if there is no stack trace. */
	pcl_err_trace_t *strace;

	/** An output buffer used by output formatting functions like pcl_err_ctx_fprintf. This is
	 * lazily-created so can be NULL. This is really just for internal use and provides for
	 * a cached/allocated buffer to use for multiple output calls. This provides no context
	 * functionality, implementation detail.
	 */
	pcl_buf_t *buffer;
};

/** Allocates a new error context. Error codes are zero, messages are all
 * NULL and the stack trace is empty.
 */
PCL_EXPORT pcl_err_ctx_t *pcl_err_ctx_create(void);

/** Clone one context to another. This performs a deep copy, no shared pointers.
 * @param src context to clone
 * @return cloned version of given context or \c NULL if \a src is \c NULL
 */
PCL_EXPORT pcl_err_ctx_t *pcl_err_ctx_clone(const pcl_err_ctx_t *src);

/** Clears the given context. A clear wipes out the stack trace and error codes.
 * However, 'ctx' is completely valid after this call.
 * @param ctx
 * @return pointer to the \a ctx argument
 */
PCL_EXPORT pcl_err_ctx_t *pcl_err_ctx_clear(pcl_err_ctx_t *ctx);

/** Release all resources held by the given context including the context itself.
 * @warning this will call ::pcl_memory_error if you try to free the per-thread context. The
 * \a type parameter of \c pcl_memory_error is set to \c "err_ctx_free".
 * @param ctx
 */
PCL_EXPORT void pcl_err_ctx_free(pcl_err_ctx_t *ctx);

/**
 *
 * @param ctx
 * @param fp
 * @param indent
 * @param message
 * @param ...
 * @return number of characters written to the given stream, which can be zero if the current
 * error is set to PCL_EOKAY. On failure, this returns -1 but DOES NOT set pcl_errno. It
 * means there was an error writing the data to the \a fp argument.
 */
PCL_EXPORT int pcl_err_ctx_fprintf(pcl_err_ctx_t *ctx, FILE *fp, int indent,
	const char *message, ...);

PCL_EXPORT int pcl_err_ctx_vfprintf(pcl_err_ctx_t *ctx, FILE *fp, int indent,
	const char *message, va_list ap);

/**
 *
 * @param ctx
 * @param buf
 * @param size
 * @param indent
 * @param message
 * @param ...
 * @return number of characters written to the given buffer or -1 on error. pcl_errno is NOT set.
 * This will return -1 when \a buf is too small.
 */
PCL_EXPORT int pcl_err_ctx_sprintf(pcl_err_ctx_t *ctx, char *buf, size_t size, int indent,
	const char *message, ...);

PCL_EXPORT int pcl_err_ctx_vsprintf(pcl_err_ctx_t *ctx, char *buf, size_t size, int indent,
	const char *message, va_list ap);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_ERRCTX_H
