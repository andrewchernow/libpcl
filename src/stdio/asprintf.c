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

#include "../string/_string.h"
#include <pcl/stdlib.h>

#undef vaxprintf
#undef axprintf
#ifdef XWIDE
	#define vaxprintf pcl_vaswprintf
	#define axprintf pcl_aswprintf
#else
	#define vaxprintf pcl_vasprintf
	#define axprintf pcl_asprintf
#endif

int
vaxprintf(xchar **out, const xchar *format, va_list ap)
{
	va_list ap2;

	if(!format)
		return -PCL_EINVAL;

	if(out)
		*out = NULL;

	va_copy(ap2, ap);
	int r = vxprintf(NULL, 0, format, ap2);
	va_end(ap2);

	if(!out || r < 0)
		return r;

	*out = (xchar *) pcl_malloc((++r) * sizeof(xchar));

	if((r = vxprintf(*out, r, format, ap)) < 0)
		*out = pcl_free(*out);

	return r;
}

int
axprintf(xchar **out, const xchar *format, ...)
{
	if(!format)
		return -PCL_EINVAL;

	va_list ap;
	va_start(ap, format);
	int r = vaxprintf(out, format, ap);
	va_end(ap);

	return r;
}

#ifndef XWIDE
	#define XWIDE

	#include "asprintf.c"

#endif
