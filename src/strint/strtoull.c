/*
  Portable C Library ("PCL")
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

#include "../string/_string.h"
#include <pcl/strint.h>
#include <stdlib.h>

#undef xstrtoull
#ifdef XWIDE
	#define xstrtoull wcstoull
#else
	#define xstrtoull strtoull
#endif

int
XFUNC(toull)(const xchar *nptr, xchar **endptr, int base, unsigned long long *result)
{
	if(strempty(nptr))
		return BADARG();

	errno = 0;
	xchar *endp = NULL;
	unsigned long long n = xstrtoull(nptr, &endp, base);

	if(endptr)
		*endptr = endp;

	if(errno == ERANGE)
		return SETOSERR(ERANGE);

	if(errno && n == 0)
		return SETOSERR(errno);

	if(nptr == endp)
		return SETERRMSG(PCL_EINVAL, "no digits founds", 0);

	if(result)
		*result = n;

	return 0;
}

#ifndef XWIDE
#	define XWIDE
#	include "strtoull.c"
#endif
