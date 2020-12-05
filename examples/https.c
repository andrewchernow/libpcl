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

#include <pcl/init.h>
#include <pcl/ssl.h>
#include <pcl/socket.h>
#include <pcl/error.h>
#include <pcl/io.h>
#include <stdlib.h>

#define DEFAULT_HOST "www.google.com"
#define REQUEST \
"GET / HTTP/1.1\r\n" \
"Host: %s\r\n" \
"Connection: close\r\n\r\n"

int main(int argc, char **argv)
{
	char request[1024];
	const char *host = argc > 1 ? argv[1] : DEFAULT_HOST;

	pcl_init();

	pcl_ssl_t *ssl = pcl_ssl_create(0);

	if(!ssl)
		PANIC("failed to create SSL socket", 0);

	if(pcl_ssl_connect(ssl, host, 443, NULL))
		PANIC("connect failed", 0);

	int r = pcl_sprintf(request, sizeof(request), REQUEST, host);

	printf("Request:\n%s", request);

	if(pcl_ssl_send(ssl, request, r) < 0)
		PANIC("sending request failed", 0);

	pcl_socket_t *sock = pcl_ssl_socket(ssl);

	/* some sites just hang forever. time them out */
	if(pcl_socket_setrcvtimeo(sock, 5000))
		PANIC("setting receive timeout", 0);

	printf("receiving ... will wait for 5 seconds\n");

	while(1)
	{
		char buf[8192];

		r = pcl_ssl_recv(ssl, buf, sizeof(buf));

		if(r > 0)
			fwrite(buf, 1, r, stdout);
		else
			break;
	}

	if(r < 0 && pcl_errno != PCL_ECONNRESET)
		PTRACE("reading response failed", 0);

	pcl_ssl_close(ssl);

	return 0;
}
