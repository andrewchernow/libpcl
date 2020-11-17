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

#include "_ssl.h"
#include <pcl/socket.h>
#include <pcl/stdlib.h>

pcl_ssl_t *
pcl_ssl_accept(pcl_ssl_t *server)
{
	pcl_ssl_t *client;
	pcl_socket_t *sock;

	if(!server || !(server->flags & PCL_SSL_PASSIVE))
		return R_SETERRMSG(NULL, PCL_EINVAL, "Not a passive SSL socket", 0);

	if(!(sock = pcl_accept(server->sock)))
		return R_TRC(NULL);

	client = pcl_zalloc(sizeof(pcl_ssl_t));
	client->flags = PCL_SSL_SERVER | (server->flags & PCL_SOCK_NONBLOCK);
	client->sock = sock;
	ipcl_ssl_configure_socket(client, NULL, 0);

	client->ssl = SSL_new(server->ctx);
	SSL_set_fd(client->ssl, (int) pcl_socket_fd(client->sock));

	return client;
}
