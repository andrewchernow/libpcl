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

/* NOTE: the errctx.h API knows nothing about per-thread contextes. It just knows how
 * to manage and output a pcl_err_t. This is why pcl_err_get() is defined within
 * the error.h API. This is the linkage.
 */

#include "_error.h"
#include <pcl/thread.h>
#include <pcl/event.h>
#include <pcl/alloc.h>
#include <pcl/buf.h>

/* holds our pcl_err_t for this thread. */
static pthread_key_t ctxkey;

/* TLS callback: ignores frozen state */
static void
ctx_destroy(void *obj)
{
	pcl_err_t *err = obj;
	ipcl_err_trace_free(err->strace);
	pcl_buf_free(err->buffer);
	pcl_free(err);
}

pcl_err_t *
pcl_err_get(void)
{
	return pcl_tls_get(ctxkey);
}

void
ipcl_err_handler(uint32_t which, void *data)
{
	UNUSED(data);

	if(which == PCL_EVENT_INIT)
		pcl_tls_alloc(&ctxkey, ctx_destroy);

	if(which == PCL_EVENT_INIT || which == PCL_EVENT_THREADINIT)
		pcl_tls_set(ctxkey, pcl_zalloc(sizeof(pcl_err_t)));
}
