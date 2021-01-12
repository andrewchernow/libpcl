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

#ifdef PCL_WINDOWS
#	include <winsock2.h>
#	include <mstcpip.h>
#endif

void
pcl_socket_setkeepalive(pcl_socket_t *sock, pcl_keepalive_t *ka)
{
	if(!sock || !ka)
		return;

#ifdef PCL_WINDOWS
	DWORD dw;
	struct tcp_keepalive buf = {0, 0, 0};

	/* enable or disable (same as SO_KEEPALIVE) */
	buf.onoff = ka->enable;

	if(ka->enable)
	{
		/* milliseconds (same as TCP_KEEPIDLE) */
		buf.keepalivetime = ka->idle * 1000;

		/* milliseconds (same as TCP_KEEPINTVL) */
		buf.keepaliveinterval = ka->interval * 1000;
	}

	/* configure keep-alives for 'conn' */
	WSAIoctl(sock->fd, SIO_KEEPALIVE_VALS, &buf, (DWORD)sizeof(buf),
		NULL, 0, &dw, NULL, NULL);

#elif defined(SO_KEEPALIVE)
	int v = ka->enable ? 1 : 0;

	v = setsockopt(sock->fd, SOL_SOCKET, SO_KEEPALIVE, (const char *) &v, sizeof(int));

	if(v == -1 || !ka->enable)
		return;

	#ifdef TCP_KEEPIDLE /* how long to wait before sending a keepalive */
	if(ka->idle > 0)
		setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPIDLE, (const char *)&ka->idle, sizeof(int));
	#endif

	#ifdef TCP_KEEPINTVL /* interval between keepalive probes */
	if(ka->interval > 0)
		setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPINTVL, (const char *) &ka->interval, sizeof(int));
	#endif

	#ifdef TCP_KEEPCNT /* how many probes before giving up */
	if(ka->probes > 0)
		setsockopt(sock->fd, IPPROTO_TCP, TCP_KEEPCNT, (const char *) &ka->probes, sizeof(int));
	#endif
#endif
}
