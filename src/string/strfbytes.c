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

#undef xsprintf
#ifdef XWIDE
	#define xsprintf pcl_swprintf
#else
	#define xsprintf pcl_sprintf
#endif

xchar *
XFUNC(fbytes)(xchar *buf, size_t bufl, uint64_t nbytes, int decimal)
{
#define KiB (double)1024
#define MiB (double)1048576
#define GiB (double)1073741824
#define TiB (double)1099511627776LL
#define PiB (double)1125899906842624LL
#define EiB (double)1152921504606846976LL

	xint unit;
	double result;
	int64_t n = (int64_t) nbytes;

	if(n < KiB)
	{
		if(xsprintf(buf, bufl, X("%d"), (int) n) < 0)
			return R_SETERR(NULL, PCL_EBUF);
		return buf;
	}

	if(n < MiB)
	{
		unit = X('K'); /* kibibyte */
		result = (double) (n / KiB);
	}
	else if(n < GiB)
	{
		unit = X('M'); /* mebibyte */
		result = (double) (n / MiB);
	}
	else if(n < TiB)
	{
		unit = X('G'); /* gibibyte */
		result = (double) (n / GiB);
	}
	else if(n < PiB)
	{
		unit = X('Y'); /* tebibyte */
		result = (double) (n / TiB);
	}
	else if(n < EiB)
	{
		unit = X('P'); /* pebibyte */
		result = (double) (n / PiB);
	}
	else
	{
		unit = X('E'); /* exbibyte */
		result = (double) (n / EiB);
	}

	if(decimal < 0)
		decimal = 0;

	decimal = min(decimal, 15);

	if(xsprintf(buf, bufl, X("%.*lf%c"), decimal, result, unit) == -1)
		return R_SETERR(NULL, PCL_EBUF);

	return buf;
}

#ifndef XWIDE
	#define XWIDE
	#include "strfbytes.c"
#endif
