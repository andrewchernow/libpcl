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
#include <pcl/limits.h>

#ifdef PCL_WINDOWS
#	include <afunix.h>
#else
#	include <sys/un.h>
#endif

int
pcl_net_host2addr(const char *host, struct sockaddr *sa)
{
	if(strempty(host))
		return BADARG();

	if(*host == '/')
	{
		if(!sa)
		{
			if(strlen(host) >= PCL_MAXUNIXSOCKPATH)
				return SETERR(PCL_EOVERFLOW);

			return 0;
		}

		struct sockaddr_un *u = (struct sockaddr_un *)sa;

		u->sun_family = AF_UNIX;

		if(!pcl_strcpy(u->sun_path, sizeof(u->sun_path), host))
			return SETERR(PCL_EOVERFLOW);

		return 0;
	}

	struct addrinfo *info = NULL;
	int r = getaddrinfo(host, NULL, NULL, &info);

	if(r)
		return SETERR(pcl_net_ai2pcl(r));

	if(sa)
		memcpy(sa, info->ai_addr, info->ai_addrlen);

	freeaddrinfo(info);
	return 0;
}
