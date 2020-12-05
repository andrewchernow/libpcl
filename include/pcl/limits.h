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

#ifndef LIBPCL_LIMITS_H
#define LIBPCL_LIMITS_H

#include <limits.h>

/** Maximum length of a unix domain socket including NUL. */
#define PCL_MAXUNIXSOCKPATH 108

/** Maximum length of an IP address: IPv4 or IPv6 */
#define PCL_MAXIPLEN 46

/** Maximum length of a domain name including NUL.
 * 63+1+63+1+63+1+62+1, see spec.
 */
#define PCL_MAXDOMAINLEN 255

#ifdef _WIN32
	#define PCL_MAXPATH 4096
	#define PCL_MAXLONGPATH 0x7fff
#else
	#define PCL_MAXPATH PATH_MAX /* 4096 linux, 1024 Darwin */
	#define PCL_MAXLONGPATH PCL_MAXPATH
#endif

#define PCL_MAXNAME 255

#endif // LIBPCL_LIMITS_H
