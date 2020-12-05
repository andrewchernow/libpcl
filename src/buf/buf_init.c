/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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

#include "_buf.h"
#include <pcl/alloc.h>
#include <string.h>

pcl_buf_t *
pcl_buf_init(pcl_buf_t *b, size_t size, enum pcl_buf_mode mode)
{
	int chrsize;

	switch(mode)
	{
		case PclBufTextW:
			chrsize = (int) sizeof(wchar_t);
			break;

		case PclBufBinary:
		case PclBufText:
			chrsize = 1;
			break;

		default:
			return R_SETERRMSG(NULL, PCL_EINVAL, "No such mode: %d", mode);
	}

	if(!b)
		b = pcl_malloc(sizeof(pcl_buf_t));

	b->mode = mode;
	b->chrsize = chrsize;
	b->len = b->pos = 0;
	b->size = size;

	if(b->size)
	{
		b->data = pcl_malloc(b->size * b->chrsize);
		memset(b->data, 0, b->chrsize);
	}

	return b;
}
