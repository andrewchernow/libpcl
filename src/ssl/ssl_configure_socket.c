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

#include "_ssl.h"
#include <pcl/socket.h>

int
ipcl_ssl_configure_socket(pcl_ssl_t *ssl, const char *host, int port)
{
	int n = 65535;

	if(host)
	{
		ssl->sock = pcl_socket(host, port);
		if(!ssl->sock)
			return TRCMSG("Failed to create socket: host=%s port=%d", host, port);
	}

	if(ssl->flags & PCL_SOCK_NONBLOCK)
		pcl_socket_setnonblocking(ssl->sock, true);

	pcl_socketfd_t fd = pcl_socket_fd(ssl->sock);

	(void) setsockopt(fd, SOL_SOCKET, SO_RCVBUF, (const char *) &n, sizeof(int));
	(void) setsockopt(fd, SOL_SOCKET, SO_SNDBUF, (const char *) &n, sizeof(int));

	if(ssl->flags & PCL_SSL_PASSIVE)
	{
		n = 1;
		(void) setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char *) &n, sizeof(int));
	}

	return 0;
}
