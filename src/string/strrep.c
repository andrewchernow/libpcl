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

#include "_string.h"

int
XFUNC(rep)(xchar *in, size_t in_max, const xchar *from, const xchar *to)
{
	xchar *p = in;
	size_t from_len, to_len, in_len;

	if(!in || !from || !to)
		return SETERR(PCL_EINVAL);

	in_len = xstrlen(in);
	from_len = xstrlen(from);
	to_len = xstrlen(to);

	do
	{
		if(!(p = xstrstr(p, from)))
			break;

		/* in buf is too small */
		if((in_len - from_len + to_len) >= in_max)
			return SETERR(PCL_EBUF);

		memmove(p + to_len, p + from_len, ((in_len - (p - in) - from_len) + 1) * sizeof(xchar));
		memcpy(p, to, to_len * sizeof(xchar));

		p += to_len;
		in_len += to_len - from_len;
	}
	while(1);

	return (int) xstrlen(in);
}

#ifndef XWIDE
	#define XWIDE
	#include "strrep.c"
#endif
