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

#ifndef LIBPCL__SOCKET_H
#define LIBPCL__SOCKET_H

#ifdef PCL_WINDOWS
#	define FD_SETSIZE 1024
#endif

#include <pcl/socket.h>

#define SOCK_ADDRBUFSIZE 128

#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_socket
{
	/* One or more PCL_SOCK_xxx flags */
	int flags;

	/* Native socket descriptor. */
	pcl_socketfd_t fd;

	/* Socket address protocol, such as IPPROTO_TCP */
	int sa_protocol;

	/* The socket type, such as SOCK_STREAM */
	int sa_socktype;

	/* cached results of first call to pcl_socket_ip() */
	char *ipaddr;

	/* Length of the socket address, 'addr'.  Initially, 128 bytes is
	 * allocated for the socket address.  However, after a connection
	 * or an accept, this gets updated to reflect the real addr len.
	 */
	int sa_len;

	/* The socket address for this socket. NOTE: Use addr->sa_family. */
	struct sockaddr *addr;
};

PCL_PRIVATE pcl_socket_t *ipcl_socket_alloc(void);

#ifdef PCL_WINDOWS
PCL_PRIVATE void ipcl_win32_socket_handler(uint32_t which, void *data);
#endif

#ifdef __cplusplus
}
#endif

#endif // LIBPCL__SOCKET_H
