
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

#ifndef LIBPCL_SOCKET_H
#define LIBPCL_SOCKET_H

/* NOTE: ---------------------------------------------------------------------------
 * PCL socket only supports AF_INET, AF_INET6 and AF_UNIX SOCK_STREAM sockets.
 */

#include <pcl/types.h>

#ifdef PCL_UNIX
#	include <poll.h>
#	include <arpa/inet.h>
#	include <sys/select.h>
#else
#	pragma warning(push, 0)
#	include <winsock2.h>
#	pragma warning(pop)
#endif

/* Socket flags: pcl_socket_t.flags */
#define PCL_SOCK_NONBLOCK   0x01
#define PCL_SOCK_CONNECTED  0x02

#ifndef INVALID_SOCKET
	#define INVALID_SOCKET (pcl_socketfd_t)(-1)
#endif

/* GNU ext. peer closed conn. or shutdown writes */
#ifndef POLLRDHUP
#	define POLLRDHUP 0x2000
#endif
#ifndef SHUT_RD
#	define SHUT_RD	  0 /* No more receptions.  */
#endif
#ifndef SHUT_WR
#	define SHUT_WR	  1	/* No more transmissions.  */
#endif
#ifndef SHUT_RDWR
#	define SHUT_RDWR	2 /* No more receptions or transmissions.  */
#endif

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	bool enable;
	int idle;
	int interval;
	int probes;
} pcl_keepalive_t;

/*
 * If host begins with a '/', a unix-domain socket is created using host as the socket
 * file pathname (limited to 100 characters). Port is not used for AF_UNIX, pcl_socket_port
 * will return 0.
 */
PCL_EXPORT pcl_socket_t *
pcl_socket(const char *host, int port);

PCL_EXPORT int
pcl_connect(pcl_socket_t *sock);

PCL_EXPORT pcl_socket_t *
pcl_accept(pcl_socket_t *sock);

/* note: timeval *is* updated, unless timeout occurs (select returns 0) or
 * timeval argument was set to NULL.
 */
PCL_EXPORT int
pcl_select(int fds, fd_set *rset, fd_set *wset, fd_set *eset, struct timeval *tv);

PCL_EXPORT void
pcl_shutdown(pcl_socket_t *sock, int how);

PCL_EXPORT ssize_t
pcl_recv(pcl_socket_t *sock, void *buf, size_t count, int flags);

PCL_EXPORT bool
pcl_recvall(pcl_socket_t *sock, void *buf, size_t count);

PCL_EXPORT ssize_t
pcl_send(pcl_socket_t *sock, const void *data, size_t count, int flags);

PCL_EXPORT bool
pcl_sendall(pcl_socket_t *sock, const void *data, size_t count);

/* For AF_UNIX, this will automatically unlink the sun_path before attempting
 * to bind the address.
 */
PCL_EXPORT int
pcl_bind(pcl_socket_t *sock);

PCL_EXPORT int
pcl_listen(pcl_socket_t *sock, int backlog);

PCL_EXPORT void
pcl_socket_close(pcl_socket_t *sock);

PCL_EXPORT bool
pcl_socket_isalive(pcl_socket_t *sock);

/* Get the last SO_ERROR from the given socket */
PCL_EXPORT int
pcl_socket_error(pcl_socket_t *sock);

PCL_EXPORT pcl_socketfd_t
pcl_socket_fd(pcl_socket_t *sock);

PCL_EXPORT char *
pcl_socket_ip(pcl_socket_t *sock);

/* For Unix Domain sockets, this always returns 0. */
PCL_EXPORT int
pcl_socket_port(pcl_socket_t *sock);

PCL_EXPORT int
pcl_socket_setrcvtimeo(pcl_socket_t *sock, uint32_t msecs);

PCL_EXPORT int
pcl_socket_setsndtimeo(pcl_socket_t *sock, uint32_t msecs);

PCL_EXPORT int
pcl_socket_setnonblocking(pcl_socket_t *sock, bool enable);

PCL_EXPORT void
pcl_socket_setkeepalive(pcl_socket_t *sock, pcl_keepalive_t *ka);

PCL_EXPORT bool
pcl_ispassive(pcl_socket_t *sock);

#ifdef __cplusplus
}
#endif
#endif

