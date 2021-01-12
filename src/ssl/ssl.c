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
#include <pcl/alloc.h>
#include <openssl/err.h>

static int
create_sslctx(pcl_ssl_t *ssl)
{
	ERR_clear_error();

	const SSL_METHOD *method;

	if(ssl->flags & (PCL_SSL_PASSIVE | PCL_SSL_SERVER))
		method = TLS_server_method();
	else
		method = TLS_client_method();

	ssl->ctx = SSL_CTX_new(method);

	if(!ssl->ctx)
		return SETERRMSG(PCL_ETYPE, "Invalid TLS method: %s", sslerror(ssl));

	SSL_CTX_set_min_proto_version(ssl->ctx, TLS1_2_VERSION);
	SSL_CTX_set_max_proto_version(ssl->ctx, TLS_MAX_VERSION);

	/* We don't need session caching.  Since we have no cache, we don't need
	 * to be concerned with SSL_ctx_set_timeout's default of 300 seconds.
	 */
	SSL_CTX_set_session_cache_mode(ssl->ctx, SSL_SESS_CACHE_OFF);

	/* Force a new session to be created for renegotiation. */
	if(ssl->flags & PCL_SSL_PASSIVE)
		SSL_CTX_set_options(ssl->ctx, SSL_OP_NO_SESSION_RESUMPTION_ON_RENEGOTIATION);

	SSL_CTX_set_verify(ssl->ctx, SSL_VERIFY_NONE, NULL);
	(void)pcl_ssl_setciphlist(ssl, PCL_SSL_DEFCIPHLIST);

	return 0;
}

pcl_ssl_t *
pcl_ssl(int flags, ...)
{
	pcl_ssl_t *ssl = pcl_zalloc(sizeof(pcl_ssl_t));

	ssl->flags = flags & (PCL_SSL_PASSIVE | PCL_SOCK_NONBLOCK);

	if(ssl->flags & PCL_SSL_PASSIVE)
	{
		va_list ap;
		const char *host;
		int port;

		va_start(ap, flags);
		host = va_arg(ap, const char *);
		port = va_arg(ap, int);
		va_end(ap);

		if(ipcl_ssl_configure_socket(ssl, host, port))
		{
			pcl_ssl_free(ssl);
			return R_TRC(NULL);
		}

		if(pcl_bind(ssl->sock))
		{
			pcl_ssl_free(ssl);
			return R_TRCMSG(NULL, "Cannot bind to port %d", port);
		}

		if(pcl_listen(ssl->sock, 50))
		{
			pcl_ssl_free(ssl);
			return R_TRCMSG(NULL, "Cannot listen on host=%s port=%d", host, port);
		}
	}

	if(create_sslctx(ssl))
	{
		pcl_ssl_free(ssl);
		return R_TRC(NULL);
	}

	return ssl;
}
