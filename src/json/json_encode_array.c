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

pcl_buf_t *
ipcl_json_encode_array(ipcl_json_encode_t *enc, pcl_vector_t *arr)
{
	pcl_buf_t *b = enc->b;

	enc->tabs++;

	pcl_buf_putchar(b, '[');

	if(enc->format && arr->count)
		pcl_buf_putchar(b, '\n');

	for(int i = 0; i < arr->count; i++)
	{
		if(enc->format)
			PRINT_TABS(enc);

		if(!ipcl_json_encode_value(enc, pcl_vector_get(arr, i)))
			return NULL;

		if(i + 1 < arr->count)
			pcl_buf_putchar(b, ',');

		if(enc->format)
			pcl_buf_putchar(b, '\n');
	}

	enc->tabs--;

	if(enc->format && arr->count)
		PRINT_TABS(enc);

	pcl_buf_putchar(b, ']');

	return b;
}
