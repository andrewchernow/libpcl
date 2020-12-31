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
#include <pcl/io.h>
#include <string.h>

int
pcl_buf_vputf(pcl_buf_t *b, const void *format, va_list ap)
{
	int len;

	/* query for formatted length */
	va_list ap2;
	va_copy(ap2, ap);
	len = b->chrsize == 1 ? pcl_vsprintf(NULL, 0, format, ap2) : pcl_vswprintf(NULL, 0, format, ap2);
	va_end(ap2);

	if(len < 0)
		return SETERR(-len);

	/* binary adds a NUL to every string */
	pcl_buf_grow(b, b->mode == PclBufBinary ? len + 1 : len);

	/* do a vsprintf buffer put */
	if(b->chrsize == 1)
		len = pcl_vsprintf(b->data + b->pos, b->size - b->pos, format, ap);
	else
		len = pcl_vswprintf(((wchar_t *) b->data) + b->pos, b->size - b->pos, format, ap);

	/* no NUL included yet */
	b->pos += len;

	if(b->mode == PclBufBinary)
	{
		memset(b->data + b->pos * b->chrsize, 0, b->chrsize); // NUL
		b->pos++;
		len++;
	}

	if(b->len < b->pos)
		b->len = b->pos;

	return len;
}
