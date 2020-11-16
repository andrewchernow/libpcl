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

#ifndef LIBPCL_TYPES_H
#define LIBPCL_TYPES_H

#include <stdint.h>
#include <sys/types.h>
#include <wchar.h>
#include <stdbool.h>

#ifdef PCL_WINDOWS
typedef wchar_t tchar_t;
typedef int mode_t;
typedef int nlink_t;
typedef int pid_t;
typedef intptr_t ssize_t;
typedef void *pcl_fd_t;
typedef uintptr_t pcl_socketfd_t;
typedef void *pcl_prochandle_t;
typedef char uid_t[68 + 1]; /* max SID string length */
typedef char gid_t[68 + 1]; /* max SID string length */
#else
typedef char tchar_t;
typedef int pcl_fd_t;
typedef int pcl_socketfd_t;
typedef pid_t pcl_prochandle_t;
#endif

typedef int64_t pcl_atomic_t;
typedef long double pcl_clock_t; /* high-res performance counter (nanosecs) */

/* forward declarations */
typedef struct tag_pcl_time pcl_time_t;
typedef struct tag_pcl_err_trace pcl_err_trace_t;
typedef struct tag_pcl_err_ctx pcl_err_ctx_t;
typedef struct tag_pcl_file pcl_file_t;
typedef struct tag_pcl_socket pcl_socket_t;
typedef struct tag_pcl_ssl pcl_ssl_t;
typedef struct tag_pcl_vector pcl_vector_t;
typedef struct tag_pcl_stat pcl_stat_t;
typedef struct tag_pcl_buf pcl_buf_t;
#ifdef PCL_WINDOWS
typedef struct tag_DIR DIR;
#endif

/** Maximum of ~37 million entries on 32-bit machines.
 * This means ~50 million entry capacity, which is 800M just for the entry table.
 * The default load factor is .75 so that means a max of ~37 million entires at 16 bytes each:
 * 592M. To max out the PCL table on 32-bit machines requires 800M + 592M = ~1.4G. That's
 * pushing the limits considering the calculation doesn't account for the memory overhead of
 * each entry's key and value: both of which are managed by the API user.
 * <p>
 * Maximum of ~1.2 billion entries on 64-bit machines.
 * This means ~1.6 billion entry capacity, which is 51.2G just for the entry table.
 * The default load factor is .75 so that means a max of ~1.2 billion entires at 32 bytes each:
 * 38.4G. To max out the PCL table on 64-bit machines requires 51.2G + 38.4G = ~89.6G.
 * Although more can be addressed on 64-bit machines, this is a crazy number of entries and
 * memory usage. This also doesn't account for the entry key and value overhead.
 * </p>
 */
typedef struct tag_pcl_htable pcl_htable_t;

typedef struct
{
	uint64_t low;
	uint64_t high;
} uint128_t;

#endif // LIBPCL_TYPES_H
