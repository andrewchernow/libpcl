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
#include <pcl/alloc.h>

pcl_json_t *
ipcl_json_parse_object(ipcl_json_state_t *s)
{
	if(*s->next++ != '{')
		JSON_THROW("expected opening object '{'", 0);

	if(!ipcl_json_skipws(s))
		return NULL;

	s->ctx = s->next;

	pcl_json_t *obj = pcl_json_object();

	/* empty object */
	if(*s->next == '}')
	{
		s->next++;
		return obj;
	}

	do
	{
		char *key = ipcl_json_parse_string(s);

		if(!key)
		{
			pcl_json_free(obj);
			return NULL;
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_json_free(obj);
			pcl_free(key);
			return NULL;
		}

		if(*s->next++ != ':')
		{
			pcl_json_free(obj);
			pcl_free(key);
			JSON_THROW("expected value ':' separator", 0);
		}

		pcl_json_t *val = ipcl_json_parse_value(s);

		if(!val)
		{
			pcl_json_free(obj);
			pcl_free(key);
			return NULL;
		}

		if(pcl_json_object_put(obj, key, val, PCL_JSON_SKIPUTF8CHK | PCL_JSON_SHALLOW) < 0)
		{
			pcl_json_free(obj);

			/* 'val' and 'key' are not managed by 'obj' cuz put failed */
			pcl_json_free(val);
			pcl_free(key);

			return NULL;
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_json_free(obj);
			return NULL;
		}

		if(*s->next == '}')
		{
			s->next++;
			return obj;
		}

		if(*s->next++ != ',')
		{
			pcl_json_free(obj);
			JSON_THROW("missing comma after value", 0);
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_json_free(obj);
			return NULL;
		}
	}
	while(s->next < s->end);

	pcl_json_free(obj);
	JSON_THROW("unexpected end of input", 0);
}
