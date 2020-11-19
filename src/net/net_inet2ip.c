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

char *
pcl_net_inet2ip(int af, const void *inaddr, char *ipbuf, size_t len)
{
	char buf[sizeof(struct sockaddr_in6)];
	struct sockaddr *sa = (struct sockaddr *)buf;

	switch((sa->sa_family = (uint16_t)af))
	{
		case AF_INET:
			((struct sockaddr_in *)sa)->sin_addr = *(struct in_addr *)inaddr;
			break;

		case AF_INET6:
			((struct sockaddr_in6 *)sa)->sin6_addr = *(struct in6_addr *)inaddr;
			break;

		default:
			return R_SETERR(NULL, PCL_EAFNOSUPPORT);
	}

	if(!pcl_net_addr2ip(sa, ipbuf, len))
		return R_TRC(NULL);

	return ipbuf;
}