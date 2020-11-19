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

#ifndef LIBPCL_STDLIB_H__
#define LIBPCL_STDLIB_H__

#include <pcl/defs.h>
#include <pcl/types.h>
#include <stdlib.h>

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

#define pcl_malloc(n) pcl_malloc_trace(n, PCL_LOCATION_ARGS)
#define pcl_zalloc(n) pcl_zalloc_trace(n, PCL_LOCATION_ARGS)
#define pcl_realloc(p, n) pcl_realloc_trace(p, n, PCL_LOCATION_ARGS)

/** Free memory space.
 * @param p memory address
 * @return always returns NULL, which is useful for assignments
 */
#define pcl_free(p)  pcl_free_trace((void*)(p), PCL_LOCATION_ARGS)

/** Safely free memory space.
 * @param p memory address, can be NULL in which case no operation is performed
 * @return always returns NULL, which is useful for assignments
 */
#define pcl_free_s(p)    ((p) ? pcl_free_trace((void*)(p), PCL_LOCATION_ARGS) : NULL)

PCL_EXPORT void *pcl_malloc_trace(size_t n, PCL_LOCATION_PARAMS);

PCL_EXPORT void *pcl_zalloc_trace(size_t n, PCL_LOCATION_PARAMS);

PCL_EXPORT void *pcl_realloc_trace(void *ptr, size_t n, PCL_LOCATION_PARAMS);

PCL_EXPORT void *pcl_free_trace(void *ptr, PCL_LOCATION_PARAMS);

/** Sets the memory error handler.
 * @param handler memory handler or NULL to reset default
 * @return the replaced handler or NULL if handler was unset
 */
PCL_EXPORT pcl_memory_handler_t pcl_set_memory_handler(pcl_memory_handler_t handler);

PCL_EXPORT void pcl_memory_error(const char *type, void *ptr, size_t siz, PCL_LOCATION_PARAMS);

/** Gets the canonicalized absolute pathname.
 * @param out absolute path is written here
 * @param outl size in characters of the out buffer
 * @param path path to convert
 * @return number of characters written to out buffer or -1 on error.
 */
PCL_EXPORT int pcl_realpath(tchar_t *out, int outl, const tchar_t *path);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtoi(const char *nptr, char **end, int base, int *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstoi(const wchar_t *nptr, wchar_t **end, int base, int *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtoui(const char *nptr, char **end, int base, unsigned int *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstoui(const wchar_t *nptr, wchar_t **end, int base, unsigned int *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtol(const char *nptr, char **end, int base, long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstol(const wchar_t *nptr, wchar_t **end, int base, long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtoul(const char *nptr, char **end, int base, unsigned long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstoul(const wchar_t *nptr, wchar_t **end, int base, unsigned long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtoll(const char *nptr, char **end, int base, long long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstoll(const wchar_t *nptr, wchar_t **end, int base, long long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_strtoull(const char *nptr, char **end, int base, unsigned long long *r);

/**
 *
 * @param nptr
 * @param end
 * @param base
 * @param result
 * @return
 */
PCL_EXPORT int pcl_wcstoull(const wchar_t *nptr, wchar_t **end, int base, unsigned long long *r);

/** Converts a tchar_t string to UTF-8.
 * @param src
 * @param src_len
 * @param lenp
 * @return
 */
PCL_EXPORT char *pcl_tcstoutf8(const tchar_t *src, size_t src_len, size_t *lenp);

/** Converts a UTF-8 string to a tchar_t string.
 */
PCL_EXPORT tchar_t *pcl_utf8totcs(const char *src, size_t src_len, size_t *lenp);

/* returns number of random bytes stored in 'buf'. This is the openssl PRNG that
 * is seeded with hardware randomness.
 */
PCL_EXPORT int pcl_rand(void *buf, int num);

PCL_EXPORT uint64_t pcl_farmhash32(const char *data);
PCL_EXPORT uint64_t pcl_farmhash32_seed(const void *data, size_t len, uint32_t seed);
PCL_EXPORT uint64_t pcl_farmhash64(const char *data);
PCL_EXPORT uint64_t pcl_farmhash64_seed(const void *data, size_t len, uint64_t seed);
PCL_EXPORT uint128_t pcl_farmhash128(const char *data);
PCL_EXPORT uint128_t pcl_farmhash128_seed(const void *data, size_t len, uint128_t seed);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_STDLIB_H__
