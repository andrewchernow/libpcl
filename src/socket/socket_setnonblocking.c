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

#include "_socket.h"
#include <pcl/error.h>

#ifdef PCL_UNIX
	#include <fcntl.h>
#endif

int
pcl_socket_setnonblocking(pcl_socket_t *sock, bool enable)
{
	int r;

	if((enable && sock->flags & PCL_SOCK_NONBLOCK) || (!enable && !(sock->flags & PCL_SOCK_NONBLOCK)))
		return 0;

#ifdef PCL_WINDOWS
	r = ioctlsocket(sock->fd, FIONBIO, (unsigned long *)&enable);

#else
	int flags = 0;

	if(fcntl(sock->fd, F_GETFL, &flags) == -1)
		return SETOSERR(pcl_sockerrno);

	if(enable)
		flags |= O_NONBLOCK;
	else
		flags &= ~O_NONBLOCK;

	r = fcntl(sock->fd, F_SETFL, flags) == -1 ? -1 : 0;
#endif

	if(r)
		return SETLASTERR();

	if(enable)
		sock->flags |= PCL_SOCK_NONBLOCK;
	else
		sock->flags &= ~PCL_SOCK_NONBLOCK;

	return r;
}
