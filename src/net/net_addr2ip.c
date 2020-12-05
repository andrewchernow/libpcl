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

#include "_net.h"
#include <pcl/string.h>

#ifdef PCL_WINDOWS
#	include <afunix.h>
#else
#	include <sys/un.h>
#endif

char *
pcl_net_addr2ip(struct sockaddr *sa, char *ipbuf, size_t len)
{
	int err;
	char *p;

	if(!sa || !ipbuf || len < 1)
		return R_SETERR(NULL, PCL_EINVAL);

	*ipbuf = 0;

	if(sa->sa_family == AF_UNIX)
	{
		struct sockaddr_un *u = (struct sockaddr_un *) sa;

		if(!pcl_strcpy(ipbuf, len, u->sun_path))
			return R_SETERR(NULL, PCL_EBUF);

		return ipbuf;
	}

	if((err = getnameinfo(sa, pcl_net_addrlen(sa), ipbuf, (uint32_t) len, NULL, 0, NI_NUMERICHOST)))
		return R_SETERR(NULL, pcl_net_ai2pcl(err));

	if(sa->sa_family == AF_INET6 && (p = strrchr(ipbuf, '%')))
		*p = 0;

	return ipbuf;
}
