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
#include <pcl/vector.h>
#include <pcl/string.h>

pcl_vector_t *
ipcl_net_resolve(const char *host)
{
	int r;
	pcl_vector_t *addrs;
	struct addrinfo *info = NULL, *ai, hints = {0};

	hints.ai_socktype = SOCK_STREAM;
	if((r = getaddrinfo(host, NULL, &hints, &info)))
		return R_SETERR(NULL, pcl_net_ai2pcl(r));

	addrs = pcl_vector_create(3, sizeof(char **), pcl_vector_cleanup_dblptr);

	for(ai=info; ai; ai=ai->ai_next)
	{
		if(ai->ai_family == AF_INET || ai->ai_family == AF_INET6)
		{
			char ip[100];

			if(getnameinfo(ai->ai_addr, (socklen_t)ai->ai_addrlen, ip, (int)sizeof(ip),
				NULL, 0, NI_NUMERICHOST) == 0)
			{
				char *a = pcl_strdup(ip);
				pcl_vector_append(addrs, &a);
			}
		}
	}

	freeaddrinfo(info);

	return addrs;
}
