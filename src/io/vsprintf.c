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

#include "../string/_string.h"
#include <pcl/alloc.h>

int
vxprintf(xchar *buf, size_t size, const xchar *format, va_list ap)
{
	if(!format)
		return -PCL_EINVAL;

	xchar fmtbuf[4096];
	xchar *fmt = xfmtconv(fmtbuf, countof(fmtbuf), format);
	int r = 0, err = 0;

	errno = 0;

#ifdef PCL_WINDOWS
	#ifdef XWIDE
	r = !buf ? _vscwprintf(fmt, ap) : _vsnwprintf_s(buf, size, size-1, fmt, ap);
	#else
	r = !buf ? _vscprintf(fmt, ap) : _vsnprintf_s(buf, size, size-1, fmt, ap);
	#endif

	if(buf && r == -1 && errno == ERANGE) /* special case, buffer too small */
		err = PCL_EBUF;

/* Unix wide */
#elif defined(XWIDE)
	if(!buf)
	{
		/* vswprintf always returns -1, write to /dev/null to get length */
		FILE *fp = fopen("/dev/null", "wb");
		r = vfwprintf(fp, fmt, ap);
		fclose(fp);
	}
	else
	{
		/* error reporting on darwin/linux is terrible. */
		r = vswprintf(buf, size, fmt, ap);
		if(r < 0 && size != 0)
			err = PCL_EBUF; /* best guess */
	}

	/* Unix non-wide */
#else
	if(!buf)
	{
		r = vsnprintf(NULL, 0, fmt, ap);
	}
	else
	{
		r = vsnprintf(buf, size, fmt, ap);
		if(r >= (int) size) /* buffer to small */
			err = PCL_EBUF;
	}
#endif

	/* set pcl error if not already set, returned as -(err) */
	if(err == 0 && r < 0)
	{
		err = pcl_err_crt2pcl(errno);
		if(err == 0 || err == PCL_EUNDEF)
			err = PCL_EINVAL;
	}

	if(fmt != fmtbuf)
		pcl_free(fmt);

	return err ? -err : r;
}

#ifndef XWIDE
	#define XWIDE
	#include "vsprintf.c"
#endif
