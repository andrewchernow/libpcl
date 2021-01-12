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

#include "_ssl.h"
#include <pcl/socket.h>
#include <openssl/err.h>

int
pcl_ssl_connect(pcl_ssl_t *ssl, const char *host, int port, int *want_io)
{
	int x;

	if(!want_io)
		want_io = &x;
	*want_io = 0;

	if(ssl->flags & (PCL_SSL_PASSIVE | PCL_SSL_SERVER))
		return SETERRMSG(PCL_EINVAL, "Not an SSL client", 0);

	if(!ssl->sock)
	{
		if(strempty(host) || port < 0 || port > 65535)
			return SETERR(PCL_EINVAL);

		if(ipcl_ssl_configure_socket(ssl, host, port))
		{
			pcl_ssl_close(ssl);
			return TRC();
		}
	}

	if(!pcl_socket_isalive(ssl->sock) && pcl_connect(ssl->sock))
	{
		if(pcl_errno != PCL_EAGAIN)
			pcl_ssl_close(ssl);
		return TRC();
	}

	if(!ssl->ssl)
	{
		ssl->ssl = SSL_new(ssl->ctx);
		SSL_set_fd(ssl->ssl, (int)pcl_socket_fd(ssl->sock));
		(void) SSL_set_tlsext_host_name(ssl->ssl, host); // SNI host
	}

	/* Negotiate */
	ERR_clear_error();
	switch(SSL_get_error(ssl->ssl, SSL_connect(ssl->ssl)))
	{
		case SSL_ERROR_NONE:
			break;

		case SSL_ERROR_ZERO_RETURN:
			pcl_ssl_close(ssl);
			return SETERR(PCL_ECONNRESET);

		case SSL_ERROR_WANT_READ:
			*want_io = PCL_SSL_WANTREAD;
			return SETERR(PCL_EAGAIN);

		case SSL_ERROR_WANT_WRITE:
			*want_io = PCL_SSL_WANTWRITE;
			return SETERR(PCL_EAGAIN);

		case SSL_ERROR_SYSCALL:
		{
			const char *e = sslerror(ssl);
			if(e)
				SETLASTERRMSG("%s", e);
			else
				SETLASTERR();
			pcl_ssl_close(ssl);
			return -1;
		}

		default:
		{
			const char *e = sslerror(ssl);
			if(e)
				SETERRMSG(PCL_ESSL, "%s", e);
			else
				SETERR(PCL_ESSL);
			pcl_ssl_close(ssl);
			return -1;
		}
	}

	if(ssl->flags & PCL_SSL_VERIFY_PEER)
	{
		long v_err;
		X509 *peer = SSL_get_peer_certificate(ssl->ssl);

		if(!peer)
		{
			pcl_ssl_close(ssl);
			return SETERRMSG(PCL_ENOTFOUND, "No certificate presented by peer", 0);
		}

		if((v_err = SSL_get_verify_result(ssl->ssl)) != X509_V_OK)
		{
			pcl_ssl_close(ssl);
			return SETERRMSG(PCL_ESSL, "[%ld] Failed to verify peer certificate", v_err);
		}
	}

	return 0;
}

