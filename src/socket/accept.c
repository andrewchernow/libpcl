/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2021 Andrew Chernow
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
#include <string.h>

#ifdef PCL_WINDOWS
#	include <ws2tcpip.h>
#endif

pcl_socket_t *
pcl_accept(pcl_socket_t *sock)
{
	int val = 0;
	pcl_socket_t *cli = ipcl_socket_alloc();

#if defined(PCL_LINUX) && defined(SOCK_NONBLOCK)
	/* Saves a fcntl call to accept the socket and set it to nonblock
	 * in a single call, only available on linux.
	 */
	val = sock->flags & PCL_SOCK_NONBLOCK ? SOCK_NONBLOCK : 0;

	cli->fd = accept4(sock->fd, (struct sockaddr *)cli->addr, (socklen_t *)&cli->sa_len, val);

	if(cli->fd != INVALID_SOCKET && val == SOCK_NONBLOCK)
		cli->flags |= PCL_SOCK_NONBLOCK;

#else
	cli->fd = accept(sock->fd, (struct sockaddr *) cli->addr, (socklen_t *) &cli->sa_len);
#endif

	if(cli->fd == INVALID_SOCKET)
	{
		int err = pcl_err_os2pcl(pcl_sockerrno);

		pcl_socket_close(cli);
		switch(err)
		{
			case PCL_EAGAIN:      /* same as EINPROGRESS, PCL_EWOULDBLOCK */
			case PCL_EINPROGRESS: /* connect would block, try again */
			case PCL_EINTR:       /* interrupted */
				err = PCL_EAGAIN;
			default:
				break;
		}

		return R_SETERR(NULL, err);
	}

	/* For some reason, accept doesn't properly populate the AF_UNIX sun_path.  So, manually
	 * copy it from server sock.
	 */
	if(cli->addr->sa_family == AF_UNIX)
	{
		memcpy(cli->addr, sock->addr, sock->sa_len);
		cli->sa_len = sock->sa_len;
	}

	cli->sa_protocol = sock->sa_protocol;
	cli->sa_socktype = sock->sa_socktype;
	cli->flags |= PCL_SOCK_CONNECTED;

	/* if accept4 was used, pcl_socket_setnonblocking does nothing since 'cli' is already
	 * flagged as nonblocking.
	 */
	if(sock->flags & PCL_SOCK_NONBLOCK)
		pcl_socket_setnonblocking(cli, true);

	setsockopt(cli->fd, SOL_SOCKET, SO_LINGER, (char *) &val, sizeof(val));
	return cli;
}
