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

#include "_error.h"
#include <pcl/buf.h>
#include <pcl/io.h>
#include <pcl/alloc.h>
#include <pcl/json.h>

pcl_json_t *
pcl_err_vjson(const char *message, va_list ap)
{
	pcl_err_t *err = pcl_err_get();
	pcl_json_t *root = pcl_json_obj();
	uint32_t flags = PCL_JSON_SKIPUTF8CHK | PCL_JSON_FREEVALONERR;
	uint32_t strflags = PCL_JSON_EMPTYASNULL | PCL_JSON_ALLOWNULL;

	if(pcl_json_objput(root, "err", pcl_json_int(err->err), flags) < 0)
	{
		pcl_json_free(root);
		return R_TRC(NULL);
	}

	char *name = (char *) (err->err ? pcl_err_name(err->err) : NULL);

	if(pcl_json_objput(root, "name", pcl_json_str(name, strflags), flags) < 0)
	{
		pcl_json_free(root);
		return R_TRC(NULL);
	}

	if(pcl_json_objput(root, "oserr", pcl_json_int(err->oserr), flags) < 0)
	{
		pcl_json_free(root);
		return R_TRC(NULL);
	}

	name = (char *) (err->oserr ? pcl_err_osname(err->oserr) : NULL);

	if(pcl_json_objput(root, "osname", pcl_json_str(name, strflags), flags) < 0)
	{
		pcl_json_free(root);
		return R_TRC(NULL);
	}

	char *msg = NULL;

	if(!strempty(message))
		pcl_vasprintf(&msg, message, ap);

	if(pcl_json_objput(root, "msg", pcl_json_str(msg, strflags | PCL_JSON_SHALLOW), flags) < 0)
	{
		pcl_json_free(root); // 'msg' managed by string & freed due to PCL_JSON_FREEVALONERR
		return R_TRC(NULL);
	}

	pcl_json_t *strace = pcl_json_arr();

	if(pcl_json_objput(root, "strace", strace, flags) < 0)
	{
		pcl_json_free(root);
		return R_TRC(NULL);
	}

	for(pcl_err_trace_t *t = err->strace; t; t = t->next)
	{
		pcl_json_t *jtrace = pcl_json_obj();

		if(pcl_json_objput(jtrace, "file", pcl_json_str(t->file, strflags), flags) < 0)
		{
			pcl_json_free(root);
			return R_TRC(NULL);
		}

		if(pcl_json_objput(jtrace, "func", pcl_json_str(t->func, strflags), flags) < 0)
		{
			pcl_json_free(root);
			return R_TRC(NULL);
		}

		if(pcl_json_objput(jtrace, "line", pcl_json_int(t->line), flags) < 0)
		{
			pcl_json_free(root);
			return R_TRC(NULL);
		}

		if(pcl_json_objput(jtrace, "msg", pcl_json_str(t->msg, strflags), flags) < 0)
		{
			pcl_json_free(root);
			return R_TRC(NULL);
		}

		if(pcl_json_arradd(strace, jtrace, flags) < 0)
		{
			pcl_json_free(root);
			return R_TRC(NULL);
		}
	}

	return root;
}
