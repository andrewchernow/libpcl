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

#include "_string.h"

static
int XFUNC(chr2bin)(const xchar hex, xchar *out)
{
	if(!out)
		return 0;

	if(hex >= X('0') && hex <= X('9'))
		*out = hex - X('0');
	else if(hex >= X('A') && hex <= X('F'))
		*out = hex - X('A') + 10;
	else if(hex >= X('a') && hex <= X('f'))
		*out = hex - X('a') + 10;
	else
		return 0;

	return 1;
}

ssize_t
XFUNC(hexdec)(void *buf, size_t size, const xchar *hex)
{
	size_t len;
	uint8_t *b = (uint8_t *) buf;

	if(!hex || !b || size < 2)
		return 0;

	*b = 0;
	len = xstrlen(hex);

	if(len < 2 || (len % 2) != 0)
		return BADARG();

	if(*hex == X('0') && hex[1] == X('x'))
		hex += 2, len -= 2;

	len /= 2;

	if(size <= len)
		return SETERR(PCL_EBUF);

	for(size_t i = 0; i < len; i++)
	{
		xchar b1, b2;

		if(!XFUNC(chr2bin)(hex[i * 2], &b1) || !XFUNC(chr2bin)(hex[i * 2 + 1], &b2))
			return SETERR(PCL_EFORMAT);

		b[i] = (uint8_t) ((b1 << 4) | b2);
	}

	return len;
}

#ifndef XWIDE
#define XWIDE
	#include "strhexdec.c"
#endif
