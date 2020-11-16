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

#include "_socket.h"
#include <pcl/error.h>
#include <pcl/string.h>
#include <pcl/net.h>

#ifdef PCL_UNIX
#include <sys/un.h>
#include <sys/socket.h>
#include <netdb.h>
#else
#	include <afunix.h>
#endif

pcl_socket_t *
pcl_socket(const char *host, int port)
{
	pcl_socket_t *sock;

	if(strempty(host) || port < 0 || port > 65535)
		return R_SETERR(NULL, PCL_EINVAL);

	sock = ipcl_socket_alloc();

	/* Unix domain socket, Windows added support in late 2017 (about time):
	 * See: https://devblogs.microsoft.com/commandline/af_unix-comes-to-windows/
	 */
	if(*host == '/')
	{
		struct sockaddr_un *u = (struct sockaddr_un *) sock->addr;

		u->sun_family = AF_UNIX;

		if(!pcl_strcpy(u->sun_path, sizeof(u->sun_path), host))
		{
			TRC();
			pcl_socket_close(sock);
			return NULL;
		}

		sock->sa_len = (int) (strlen(u->sun_path) + sizeof(u->sun_family));
		sock->sa_protocol = 0;
		sock->sa_socktype = SOCK_STREAM;
	}
	else
	{
		struct addrinfo *info = NULL, *ai, hints = {0};

		hints.ai_socktype = SOCK_STREAM;

		if(!strcmp(host, "::"))
			hints.ai_flags = AI_PASSIVE;

		int r = getaddrinfo(host, NULL, &hints, &info);

		if(r)
		{
			SETERR(pcl_net_ai2pcl(r));
			pcl_socket_close(sock);
			return NULL;
		}

		/* We only support AF_INET/INET6 SOCK_STREAM addresses */
		for(ai = info; ai; ai = ai->ai_next)
			if(ai->ai_family == AF_INET || ai->ai_family == AF_INET6)
				break;

		if(!ai)
		{
			freeaddrinfo(info);
			SETERRMSG(PCL_EPFNOSUPPORT, "Cannot find suitable IPv4 or IPv6 socket stream address", 0);
			pcl_socket_close(sock);
			return NULL;
		}

		sock->sa_len = (int) ai->ai_addrlen;
		sock->sa_protocol = ai->ai_protocol;
		sock->sa_socktype = SOCK_STREAM;
		memcpy(sock->addr, ai->ai_addr, ai->ai_addrlen);

		freeaddrinfo(info);

		/* set the port */
		if(sock->addr->sa_family == AF_INET)
		{
			((struct sockaddr_in *) sock->addr)->sin_port = htons((uint16_t) port);
		}
		else if(sock->addr->sa_family == AF_INET6)
		{
			((struct sockaddr_in6 *) sock->addr)->sin6_port = htons((uint16_t) port);
		}
		else
		{
			SETERR(PCL_EAFNOSUPPORT);
			pcl_socket_close(sock);
			return NULL;
		}
	}

	sock->fd = socket(sock->addr->sa_family, sock->sa_socktype, sock->sa_protocol);

	if(sock->fd == INVALID_SOCKET)
	{
		SETLASTERR();
		pcl_socket_close(sock);
		return NULL;
	}

	return sock;
}

