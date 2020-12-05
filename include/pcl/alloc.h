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

#ifndef LIBPCL_ALLOC_H
#define LIBPCL_ALLOC_H

/** @defgroup alloc Allocation Functions
 * A set of function for managing memory.
 * @{
 */
#include <pcl/types.h>
#include <pcl/defs.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Prototype for a memory handler.
 * @param type the type of memory operation that encounted the error: pcl_malloc, strdup, etc.
 * @param ptr memory address used during failed operation
 * @param siz size in bytes of memory space used during failed operation
 * @param file file name where the error took place
 * @param func function name where the error took placce
 * @param line line number within file where error took place
 */
typedef void (*pcl_memory_handler_t)(const char *type, void *ptr, size_t siz, PCL_LOCATION_PARAMS);

#ifdef __doxygen__
	/** @copydoc pcl_malloc_trace
	 * @note implemented as a macro
	 */
	void *pcl_malloc(size_t n);

	/** @copydoc pcl_zalloc_trace
	 * @note implemented as a macro
	 */
	void *pcl_zalloc(size_t n);

	/** @copydoc pcl_realloc_trace
	 * @note implemented as a macro
	 */
	void *pcl_realloc(void *p, size_t n);

	/** @copydoc pcl_free_trace
	 * @note implemented as a macro
	 */
	void *pcl_free(void *p);

	/** Safely free memory space.
	 * @param p memory address, can be NULL in which case no operation is performed
	 * @return always returns NULL, which is useful for assignments
	 * @note implemented as a macro
	 */
	void *pcl_free_safe(void *p);
#else
#	define pcl_malloc(n) pcl_malloc_trace(n, PCL_LOCATION_ARGS)
#	define pcl_zalloc(n) pcl_zalloc_trace(n, PCL_LOCATION_ARGS)
#	define pcl_realloc(p, n) pcl_realloc_trace(p, n, PCL_LOCATION_ARGS)
#	define pcl_free(p) pcl_free_trace((void*)(p), PCL_LOCATION_ARGS)
#	define pcl_free_safe(p) ((p) ? pcl_free_trace((void*)(p), PCL_LOCATION_ARGS) : NULL)
#endif

/** Allocate dynamic memory.
 * @param n size in bytes
 * @return address of allocate memory
 */
PCL_EXPORT void *pcl_malloc_trace(size_t n, PCL_LOCATION_PARAMS);

/** Allocate and zero dynamic memory.
 * @param n size in bytes
 * @return address of allocate memory
 */
PCL_EXPORT void *pcl_zalloc_trace(size_t n, PCL_LOCATION_PARAMS);

/** Re-allocate dynamic memory.
 * @param p address of allocated memory
 * @param n new size in bytes
 * @return address of allocated memory
 */
PCL_EXPORT void *pcl_realloc_trace(void *p, size_t n, PCL_LOCATION_PARAMS);

/** Free memory space.
 * @param p memory address
 * @return always returns NULL, which is useful for assignments
 */
PCL_EXPORT void *pcl_free_trace(void *p, PCL_LOCATION_PARAMS);

/** Set the process memory error handler.
 * @param handler new memory error handler
 * @return previous memory error handler or \c NULL if it was unset
 */
PCL_EXPORT pcl_memory_handler_t pcl_set_memory_handler(pcl_memory_handler_t handler);

/** Execute process memory error handler. */
PCL_EXPORT void pcl_memory_error(const char *type, void *ptr, size_t siz, PCL_LOCATION_PARAMS);

#ifdef __doxygen__
	/** Allocate dynamic memory.
	 * @param n size in ::tchar_t characters
	 * @return address of allocate memory
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tmalloc(size_t n);

	/** Allocate and zero dynamic memory.
	 * @param n size in ::tchar_t characters
	 * @return address of allocate memory
	 * @note implemented as a macro
	 */
	tchar_t *pcl_tzalloc(size_t n);

	/** Re-allocate dynamic memory.
	 * @param p address of allocated memory
	 * @param n new size in ::tchar_t characters
	 * @return address of allocated memory
	 * @note implemented as a macro
	 */
	tchar_t *pcl_trealloc(void *p, size_t n);
#else
#	define pcl_tmalloc(len) (tchar_t *) pcl_malloc((len) * sizeof(tchar_t))
#	define pcl_tzalloc(len) (tchar_t *) pcl_zalloc((len) * sizeof(tchar_t))
#	define pcl_trealloc(ptr, len) (tchar_t *) pcl_realloc(ptr, (len) * sizeof(tchar_t))
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_ALLOC_H
