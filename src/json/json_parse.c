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

#include "_json.h"
#include <pcl/error.h>
#include <string.h>

pcl_json_value_t *
pcl_json_parse(const char *json, size_t len)
{
	if(!json)
		return R_SETERR(NULL, PCL_EINVAL);

	if(len == 0)
		len = strlen(json);

	if(len == 0)
		return R_SETERRMSG(NULL, PCL_EINVAL, "empty json string", 0);

	/* skip utf8 BOM byte order mark */
	if(len >= 3 && memcmp(json, "\xEF\xBB\xBF", 3) == 0)
	{
		len -= 3;
		json += 3;
	}

	if(len == 0)
		return R_SETERRMSG(NULL, PCL_EINVAL, "empty json string", 0);

	ipcl_json_state_t state = {
		.next = json,
		.end = json + len,
		.ctx = json,
		.line = 1
	};

	/* implemented as a recursive descent parser, this kicks off the recursion */
	pcl_json_value_t *val = ipcl_json_parse_value(&state, NULL);

	if(val)
		return val;

	/* prepare error: try to show some context around the error site */
	len = state.end - state.ctx;
	state.end = state.ctx + min(len, 16);

	char ctxbuf[40];
	char *ctx = ctxbuf;

	/* avoid special characters messing up stack trace */
	for(; state.ctx < state.end; state.ctx++)
	{
		switch(*state.ctx)
		{
			case '\f':
				*ctx++ = '\\';
				*ctx++ = 'f';
				break;

			case '\t':
				*ctx++ = '\\';
				*ctx++ = 't';
				break;

			case '\r':
				*ctx++ = '\\';
				*ctx++ = 'r';
				break;

			case '\n':
				*ctx++ = '\\';
				*ctx++ = 'n';
				break;

			case '\b':
				*ctx++ = '\\';
				*ctx++ = 'b';
				break;

			default:
				*ctx++ = *state.ctx;
		}
	}

	*ctx = 0;

	return R_TRCMSG(NULL, "line=%d, context=%s", state.line, ctxbuf);
}
