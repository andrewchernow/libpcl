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

#include "_net.h"

int
pcl_net_ai2pcl(int ai_retval)
{
	switch(ai_retval)
	{
#ifdef EAI_BADFLAGS
		case EAI_BADFLAGS:
			return PCL_EINVAL;
#endif

#ifdef EAI_NONAME
		case EAI_NONAME:
			return PCL_EHOSTNOTFOUND;
#endif

#ifdef EAI_AGAIN
		case EAI_AGAIN:
			return PCL_ETRYAGAIN;
#endif

#ifdef EAI_FAIL
		case EAI_FAIL:
			return PCL_EHOSTNOTFOUND;
#endif

#if defined(EAI_NODATA) && EAI_NODATA != EAI_NONAME
		case EAI_NODATA:
			return PCL_ENODATA;
#endif

#ifdef EAI_FAMILY
		case EAI_FAMILY:
			return PCL_EAFNOSUPPORT;
#endif

#ifdef EAI_SOCKTYPE
		case EAI_SOCKTYPE:
			return PCL_ESOCKTNOSUPPORT;
#endif

#ifdef EAI_SERVICE
		case EAI_SERVICE:
			return PCL_ESOCKTNOSUPPORT;
#endif

#ifdef EAI_ADDRFAMILY
		case EAI_ADDRFAMILY:
			return PCL_EAFNOSUPPORT;
#endif

#ifdef EAI_MEMORY
		case EAI_MEMORY:
			return PCL_ENOMEM;
#endif

#ifdef EAI_SYSTEM
		case EAI_SYSTEM:
			return pcl_err_os2pcl(errno);
#endif

#ifdef EAI_OVERFLOW
		case EAI_OVERFLOW:
			return PCL_EOVERFLOW;
#endif

#ifdef EAI_INPROGRESS
		case EAI_INPROGRESS:
			return PCL_EINPROGRESS;
#endif

#ifdef EAI_CANCELED
		case EAI_CANCELED:
			return PCL_ECANCELLED;
#endif

#ifdef EAI_NOTCANCELED
		case EAI_NOTCANCELED:
			return PCL_EINPROGRESS;
#endif

#ifdef EAI_ALLDONE
		case EAI_ALLDONE:
			return PCL_EOKAY;
#endif

#ifdef EAI_INTR
		case EAI_INTR:
			return PCL_EINTR;
#endif

#ifdef EAI_IDN_ENCODE
		case EAI_IDN_ENCODE:
			return PCL_EFORMAT;
#endif

		default:
			break;
	}

	return PCL_EUNDEF;
}

