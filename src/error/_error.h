/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2021 Andrew Chernow
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

#ifndef LIBPCL__ERROR_H
#define LIBPCL__ERROR_H

#include <pcl/error.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	int err;
	const char *name;
	const char *msg;
} ipcl_err_msgmap_t;

typedef struct
{
	uint32_t oserr;       /* The OS error code. */
	int err;              /* The PCL error code. */
	const char *osname;   /* The OS error code name. */
} ipcl_err_map_t;

PCL_PRIVATE ipcl_err_msgmap_t *ipcl_err_msgmap(int *count);
PCL_PRIVATE ipcl_err_map_t *ipcl_err_map(int *count);
PCL_PRIVATE pcl_buf_t *ipcl_err_serialize(int indent, const char *format, va_list ap);
PCL_PRIVATE void ipcl_err_handler(uint32_t which, void *data);
PCL_PRIVATE pcl_err_trace_t *ipcl_err_trace_free(pcl_err_trace_t *head);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL__ERROR_H
