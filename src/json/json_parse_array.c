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
#include <pcl/array.h>
pcl_json_t *
ipcl_json_parse_array(ipcl_json_state_t *s)
{
	if(*s->next++ != '[')
		JSON_THROW("expected opening array '['", 0);

	ipcl_json_skipws(s);
	s->ctx = s->next;

	pcl_json_t *arr = pcl_json_arr();

	/* empty array */
	if(*s->next == ']')
	{
		s->next++;
		return arr;
	}

	do
	{
		pcl_json_t *val = ipcl_json_parse_value(s);

		if(!val)
		{
			pcl_json_free(arr);
			return NULL;
		}

		if(pcl_json_arradd(arr, val, PCL_JSON_FREEVALONERR) < 0)
		{
			pcl_json_free(arr);
			return R_TRCMSG(NULL, "failed to add value to array", 0);
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_json_free(arr);
			return NULL;
		}

		if(*s->next == ']')
		{
			s->next++;
			return arr;
		}

		if(*s->next++ != ',')
		{
			pcl_json_free(arr);
			JSON_THROW("missing comma after value", 0);
		}

		if(!ipcl_json_skipws(s))
		{
			pcl_json_free(arr);
			return NULL;
		}
	}
	while(s->next < s->end);

	pcl_json_free(arr);
	JSON_THROW("unexpected end of input", 0);
}
