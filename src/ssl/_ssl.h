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

#ifndef LIBPCL__SSL_H
#define LIBPCL__SSL_H

#include <pcl/ssl.h>
#include <pcl/error.h>
#include <openssl/ssl.h>

/* internal use, used with SETERRMSG() like macros */
#define sslerror(_ssl) pcl_ssl_error(_ssl)->msg

#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_ssl
{
	/* PCL_SSL_xxx, should not be modified by applications.  The PCL_SOCK_NONBLOCK flag is copied to
	 * clients during http_accept(). Thus, the values configured on passive objects (PCL_SSL_PASSIVE)
	 * dictate how new clients are configured.
	 */
	int flags;
	pcl_socket_t *sock;
	SSL_CTX *ctx;
	SSL *ssl;
	pcl_openssl_error_t err;
};

PCL_PRIVATE int ipcl_ssl_configure_socket(pcl_ssl_t *ssl, const char *host, int port);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL__SSL_H
