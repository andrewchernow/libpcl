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
#include <string.h>
#include <pcl/json.h>
#include <pcl/array.h>

int
pcl_err_setjson(pcl_json_t *error)
{
	pcl_err_t *err = pcl_err_get();

	if(err->frozen)
		return 0;

	if(!pcl_json_isobj(error))
		return SETERR(PCL_ETYPE);

	long long errcode = pcl_json_objgetint(error, "err");
	long long oscode = pcl_json_objgetint(error, "oserr");
	pcl_json_t *strace = pcl_json_objget(error, "strace");

	if(errcode == PCL_JSON_INVINT || oscode == PCL_JSON_INVINT || !pcl_json_isarr(strace))
		return SETERR(PCL_EFORMAT);

	pcl_err_clear();

	/* ignore errorrs while building pcl_err_t */
	err->frozen = true;
	err->err = (int) errcode;
	err->oserr = (uint32_t) oscode;

	for(int i = pcl_json_count(strace) - 1; i >= 0; i--)
	{
		pcl_json_t *trc = pcl_json_arrget(strace, i);

		if(pcl_json_isobj(trc))
		{
			const char *file = pcl_json_objgetstr(trc, "file");
			const char *func = pcl_json_objgetstr(trc, "func");
			long long line = pcl_json_objgetint(trc, "line");
			const char *msg = pcl_json_objgetstr(trc, "msg");

			if(line == PCL_JSON_INVINT)
				line = 0;

			pcl_err_trace(file, func, (int) line, msg);
		}
	}

	err->frozen = false;
	return 0;
}
