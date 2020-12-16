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
#include <pcl/string.h>
#include <pcl/alloc.h>

pcl_json_t *
pcl_json_string(char *str, size_t len, uint32_t flags)
{
	if(!str)
		return R_SETERR(NULL, PCL_EINVAL);

	if(!(flags & PCL_JSON_SKIPUTF8CHK) && ipcl_json_utf8check(str, len) < 0)
		return R_TRC(NULL);

	pcl_json_t *val = pcl_malloc(sizeof(pcl_json_t));

	val->type = 's';

	if(flags & PCL_JSON_SHALLOW)
	{
		/* must be NUL-terminated, documented as such */
		if(len == 0)
		{
			val->string = str;
		}
		else
		{
			/* json strings MUST be NUL-terminated, ensure we can add a NUL. In many cases,
			 * the realloc is a no-op since most string allocations are larger than "len".
			 */
			val->string = pcl_realloc(str, len + 1);
			val->string[len] = 0;
		}
	}
	else if(len == 0)
	{
		val->string = pcl_strdup(str);
	}
	else
	{
		val->string = pcl_strndup(str, len);
	}

	return val;
}
