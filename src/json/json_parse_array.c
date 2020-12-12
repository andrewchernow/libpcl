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
#include <pcl/vector.h>

static void
value_cleanup(pcl_vector_t *v, void *elem)
{
	UNUSED(v);
	ipcl_json_value_clear(elem);
}

pcl_vector_t *
ipcl_json_parse_array(ipcl_json_state_t *s)
{
	if(*s->next != '[')
		JSON_THROW("expected opening array '['", 0);

	s->next++;
	ipcl_json_skipws(s);
	s->ctx = s->next;

	pcl_vector_t *arr = pcl_vector_create(8, sizeof(pcl_json_value_t), value_cleanup);

	/* empty array */
	if(*s->next == ']')
	{
		s->next++;
		return arr;
	}

	do
	{
		pcl_json_value_t valbuf;
		pcl_json_value_t *val = ipcl_json_parse_value(s, &valbuf);

		if(!val)
		{
			pcl_vector_free(arr);
			return NULL;
		}

		if(!pcl_vector_append(arr, val))
		{
			TRCMSG("failed to add value to array", 0);
			pcl_vector_free(arr);
			ipcl_json_value_clear(val); // not managed by vector
			return NULL;
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_vector_free(arr);
			return NULL;
		}

		if(*s->next == ']')
		{
			s->next++;
			return arr;
		}

		if(*s->next != ',')
		{
			pcl_vector_free(arr);
			JSON_THROW("missing comma after value", 0);
		}

		s->next++;

		if(!ipcl_json_skipws(s))
		{
			pcl_vector_free(arr);
			return NULL;
		}
	}
	while(s->next < s->end);

	pcl_vector_free(arr);
	JSON_THROW("unexpected end of input", 0);
}
