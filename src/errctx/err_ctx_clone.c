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

#include "_errctx.h"
#include <pcl/alloc.h>
#include <string.h>

pcl_err_ctx_t *
pcl_err_ctx_clone(const pcl_err_ctx_t *src)
{
	if(!src)
		return NULL;

	pcl_err_ctx_t *dest = pcl_err_ctx_create();

	/* note: do not clone ctx->buffer. That is just a cache for serialize */
	dest->err = src->err;
	dest->oserr = src->oserr;

	/* clone src stack trace */
	for(pcl_err_trace_t *tail = NULL, *current = src->strace; current; current = current->next)
	{
		pcl_err_trace_t *node = pcl_malloc(current->size);

		/* first copy entire block of memory */
		memcpy(node, current, current->size);

		/* repoint file, func and msg */
		size_t file_len = current->file ? strlen(current->file) + 1 : 0;
		size_t func_len = current->func ? strlen(current->func) + 1 : 0;
		size_t msg_len = current->msg ? strlen(current->msg) + 1 : 0;
		char *end = (char *) (node + 1);

		node->file = file_len ? end : NULL;
		node->func = func_len ? end + file_len : NULL;
		node->msg = msg_len ? end + file_len + func_len : NULL;
		node->next = NULL;

		/* add to list */
		if(!dest->strace)
			tail = dest->strace = node;
		else
			tail = tail->next = node;
	}

	return dest;
}
