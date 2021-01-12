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

int
pcl_connect(pcl_socket_t *sock)
{
	int r = 0, err;

	/* already connected */
	if(sock->flags & PCL_SOCK_CONNECTED)
		return 0;

	if(sock->fd == INVALID_SOCKET)
		return SETERR(PCL_EINVAL);

	/* connect succeeded */
	if(!connect(sock->fd, sock->addr, sock->sa_len))
	{
		sock->flags |= PCL_SOCK_CONNECTED;
		return 0;
	}

	err = pcl_err_os2pcl(pcl_sockerrno);

	switch(err)
	{
		case PCL_EISCONN:     /* already connected */
			sock->flags |= PCL_SOCK_CONNECTED;
			break;

		case PCL_EALREADY:    /* previous connect not yet finished */
		case PCL_EAGAIN:      /* same as EINPROGRESS, PCL_EWOULDBLOCK */
		case PCL_EINPROGRESS: /* connect would block, try again */
		case PCL_EINTR:       /* interrupted */
			r = SETERR(PCL_EAGAIN);
			break;

		default:
			r = SETERR(err);
	}

	return r;
}
