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
#include <pcl/alloc.h>
#include <string.h>

pcl_json_t *
ipcl_json_parse_value(ipcl_json_state_t *s)
{
	pcl_json_t *val;

	s->ctx = s->next;

	if(!ipcl_json_skipws(s))
		return NULL;

	s->ctx = s->next;

	switch(*s->next)
	{
		case '{':
		{
			if(!(val = ipcl_json_parse_object(s)))
				return NULL;
			break;
		}

		case '[':
		{
			if(!(val = ipcl_json_parse_array(s)))
				return NULL;
			break;
		}

		case '"':
		{
			char *str = ipcl_json_parse_string(s);

			if(!str)
				return NULL;

			/* ipcl_json_parse_string already did UTF-8 check */
			val = pcl_json_string(str, 0, PCL_JSON_SKIPUTF8CHK | PCL_JSON_SHALLOW);
			break;
		}

		case 't':
		{
			if(s->end - s->next < 4 || strncmp(s->next + 1, "rue", 3) != 0)
				JSON_THROW("invalid json value", 0);

			s->next += 4;
			val = pcl_json_true();
			break;
		}

		case 'f':
		{
			if(s->end - s->next < 5 || strncmp(s->next + 1, "alse", 4) != 0)
				JSON_THROW("invalid json value", 0);

			s->next += 5;
			val = pcl_json_false();
			break;
		}

		case 'n':
		{
			if(s->end - s->next < 4 || strncmp(s->next + 1, "ull", 3) != 0)
				JSON_THROW("invalid json value", 0);

			s->next += 4;
			val = pcl_json_null();
			break;
		}

		case '0':
		case '-':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		{
			if(!(val = ipcl_json_parse_number(s)))
				return NULL;
			break;
		}

		default:
			JSON_THROW("expected json value", 0);
	}

	return val;
}
