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
#include <pcl/htable.h>
#include <pcl/string.h>
#include <pcl/alloc.h>

int
pcl_json_objput(pcl_json_t *obj, char *key, pcl_json_t *value, uint32_t flags)
{
	uint32_t freeval = flags & PCL_JSON_FREEVALONERR;

	if(!obj || !key || !value)
	{
		if(value && freeval)
			pcl_json_free(value);

		return BADARG();
	}

	if(!pcl_json_isobj(obj))
	{
		if(freeval)
			pcl_json_free(value);

		return SETERRMSG(PCL_ETYPE, "expected type 'o', got '%c'", obj->type);
	}

	if(!(flags & PCL_JSON_SKIPUTF8CHK) && ipcl_json_utf8check(key, 0) < 0)
	{
		if(freeval)
			pcl_json_free(value);

		return TRC();
	}

	char *k = key;

	if(!(flags & PCL_JSON_SHALLOW))
		k = pcl_strdup(key);

	if(pcl_htable_put(obj->object, k, value, true) < 0)
	{
		if(k != key)
			pcl_free(k);

		if(freeval)
			pcl_json_free(value);

		return TRCMSG("failed to put '%s' key", key);
	}

	return 0;
}
