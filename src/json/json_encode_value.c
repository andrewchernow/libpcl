/*
  Portable C Library (PCL)
  Copyright (c) 1999-2021 Andrew Chernow
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
#include <math.h>

pcl_buf_t *
ipcl_json_encode_value(ipcl_json_encode_t *enc, pcl_json_t *value)
{
	pcl_buf_t *b = enc->b;

	switch(value->type)
	{
		case 0:
			pcl_buf_putstr(b, "null");
			break;

		case 'o':
			if(!ipcl_json_encode_object(enc, value->object))
				return NULL;
			break;

		case 'a':
			if(!ipcl_json_encode_array(enc, value->array))
				return NULL;
			break;

		case 'b':
			pcl_buf_putstr(b, value->boolean ? "true" : "false");
			break;

		case 's':
			if(!ipcl_json_encode_string(enc, value->string))
				return NULL;
			break;

		case 'i':
			pcl_buf_putf(b, "%lld", value->integer);
			break;

		case 'r':
		{
			if(isnan(value->real) || isinf(value->real))
				pcl_buf_putstr(b, "null");
			else
				pcl_buf_putf(b, "%1.15g", value->real);

			break;
		}

		default:
			return R_SETERRMSG(NULL, PCL_ETYPE, "invalid json type '%c'", value->type);
	}

	return b;
}
