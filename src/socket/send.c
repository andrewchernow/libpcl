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

ssize_t
pcl_send(pcl_socket_t *sock, const void *data, size_t count, int flags)
{
#ifdef PCL_WINDOWS
	int r;
	int cnt = (int)count;
#else
	ssize_t r;
	size_t cnt = count;
#endif

	if(!pcl_socket_isalive(sock) || !data)
		return SETERR(PCL_EINVAL);

	r = send(sock->fd, data, cnt, flags);

	if(r == -1)
	{
		int err = pcl_err_os2pcl(pcl_sockerrno);

		/* give an interrupt one more try */
		if(err == PCL_EINTR)
			r = send(sock->fd, data, cnt, flags);

		if(r == -1)
		{
			err = pcl_err_os2pcl(pcl_sockerrno);

			if(err == PCL_EWOULDBLOCK || err == PCL_EINTR)
				err = PCL_EAGAIN;

			return SETERR(err);
		}
	}

	return r;
}
