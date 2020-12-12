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

static void
remove_entry(const void *key, void *value, void *userp)
{
	UNUSED(userp);
	pcl_free(key);
	ipcl_json_value_clear(value);
	pcl_free(value);
}

pcl_htable_t *
ipcl_json_parse_object(ipcl_json_state_t *s)
{
	if(*s->next != '{')
		JSON_THROW("expected opening object '{'", 0);

	s->next++;

	if(!ipcl_json_skipws(s))
		return NULL;

	s->ctx = s->next;

	pcl_htable_t *obj = pcl_htable_create(0);
	obj->remove_entry = remove_entry;

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
			pcl_htable_free(obj);
			return NULL;
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_htable_free(obj);
			pcl_free(key);
			return NULL;
		}

		if(*s->next != ':')
		{
			pcl_htable_free(obj);
			pcl_free(key);
			JSON_THROW("expected value ':' separator", 0);
		}

		s->next++;
		pcl_json_value_t *jv = ipcl_json_parse_value(s, NULL);

		if(!jv)
		{
			pcl_htable_free(obj);
			pcl_free(key);
			return NULL;
		}

		if(pcl_htable_put(obj, key, jv, true) < 0)
		{
			TRCMSG("object '%s' put failed", key);

			pcl_htable_free(obj);

			/* 'jv' and 'key' are not managed by 'obj' cuz put failed */
			ipcl_json_value_clear(jv);
			pcl_free(jv);
			pcl_free(key);

			return NULL;
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_htable_free(obj);
			return NULL;
		}

		if(*s->next == '}')
		{
			s->next++;
			return obj;
		}

		if(*s->next != ',')
		{
			pcl_htable_free(obj);
			JSON_THROW("missing comma after value", 0);
		}

		s->next++;

		if(!ipcl_json_skipws(s))
		{
			pcl_htable_free(obj);
			return NULL;
		}
	}
	while(s->next < s->end);

	pcl_htable_free(obj);
	JSON_THROW("unexpected end of input", 0);
}
