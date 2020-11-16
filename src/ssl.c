
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

#include <pcl/ssl.h>
#include <pcl/socket.h>
#include <pcl/vector.h>
#include <pcl/error.h>
#include <pcl/stdlib.h>
#include <pcl/string.h>
#include <pcl/stdio.h>
#include <openssl/err.h>
#include <openssl/ssl.h>

/* internal use, used with SETERRMSG() like macros */
#define sslerror(_ssl) pcl_ssl_error(_ssl)->msg

struct tag_pcl_ssl
{
	/* PCL_SSL_xxx, should not be modified by applications.  The
	 * PCL_SOCK_NONBLOCK flag is copied to clients during http_accept().
	 * Thus, the values configured on passive objects (PCL_SSL_PASSIVE)
	 * dictate how new clients are configured.
	 */
	int flags;
	pcl_socket_t *sock;
	SSL_CTX *ctx;
	SSL *ssl;
	pcl_openssl_error_t err;
};

static int create_sslctx(pcl_ssl_t *ssl);
static int configure_socket(pcl_ssl_t *ssl, const char *host, int port);

pcl_ssl_t *
pcl_ssl_create(int flags, ...)
{
	pcl_ssl_t *ssl = (pcl_ssl_t *)pcl_zalloc(sizeof(pcl_ssl_t));

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

		if(configure_socket(ssl, host, port))
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

void
pcl_ssl_close(pcl_ssl_t *ssl)
{
	if(!ssl)
		return;

	if(ssl->ssl)
	{
		SSL_shutdown(ssl->ssl);
		SSL_free(ssl->ssl);
		ssl->ssl = NULL;
	}

	if(ssl->err.msg)
		*ssl->err.msg = 0;

	pcl_socket_close(ssl->sock);
	ssl->sock = NULL;
	ERR_clear_error();
}

void
pcl_ssl_free(pcl_ssl_t *ssl)
{
	if(!ssl)
		return;

	pcl_ssl_close(ssl);
	SSL_CTX_free(ssl->ctx);
	pcl_free_s(ssl->err.msg);
	pcl_free(ssl);
}

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

		if(configure_socket(ssl, host, port))
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

void *
pcl_ssl_peercert(pcl_ssl_t *ssl)
{
	return ssl ? SSL_get_peer_certificate(ssl->ssl) : NULL;
}

pcl_ssl_t *
pcl_ssl_accept(pcl_ssl_t *server)
{
	pcl_ssl_t *client;
	pcl_socket_t *sock;

	if(!server || !(server->flags & PCL_SSL_PASSIVE))
		return R_SETERRMSG(NULL, PCL_EINVAL, "Not a passive SSL socket", 0);

	if(!(sock = pcl_accept(server->sock)))
		return R_TRC(NULL);

	client = (pcl_ssl_t *)pcl_zalloc(sizeof(pcl_ssl_t));
	client->flags = PCL_SSL_SERVER | (server->flags & PCL_SOCK_NONBLOCK);
	client->sock = sock;
	configure_socket(client, NULL, 0);

	client->ssl = SSL_new(server->ctx);
	SSL_set_fd(client->ssl, (int)pcl_socket_fd(client->sock));

	return client;
}

int
pcl_ssl_handshake(pcl_ssl_t *ssl, int *want_io)
{
	int x;

	if(!want_io)
		want_io = &x;
	*want_io = 0;

	if(!(ssl->flags & PCL_SSL_SERVER))
		return SETERR(PCL_EINVAL);

	ERR_clear_error();
	switch(SSL_get_error(ssl->ssl, SSL_accept(ssl->ssl)))
	{
		case SSL_ERROR_NONE:
			break;

		case SSL_ERROR_ZERO_RETURN:
			SETERRMSG(PCL_ECONNRESET, "%s", sslerror(ssl));
			pcl_ssl_close(ssl);
			return -1;

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

	return 0;
}

pcl_vector_t *
pcl_ssl_getcertentry(pcl_ssl_t *ssl, int entry_type)
{
	X509 *cert;
	X509_NAME *name;
	int nid;
	int pos = -1;
	pcl_vector_t *ents = NULL;

	if(ssl == NULL)
	{
		BADARG();
		return NULL;
	}

	cert = SSL_get_peer_certificate(ssl->ssl);
	name = X509_get_subject_name(cert);

	switch(entry_type)
	{
		case PCL_SSL_CERT_COUNTRY_NAME:
			nid = NID_countryName;
			break;

		case PCL_SSL_CERT_LOCALITY_NAME:
			nid = NID_localityName;
			break;

		case PCL_SSL_CERT_STATE:
			nid = NID_stateOrProvinceName;
			break;

		case PCL_SSL_CERT_ORG_NAME:
			nid = NID_organizationName;
			break;

		case PCL_SSL_CERT_ORG_UNIT:
			nid = NID_organizationalUnitName;
			break;

		case PCL_SSL_CERT_COMMON_NAME:
			nid = NID_commonName;
			break;

		default:
			SETERRMSG(PCL_ETYPE, "Unknown certificate field: %d", entry_type);
			return NULL;
	}

	while(true)
	{
		ASN1_STRING *str;
		char *value;

		pos = X509_NAME_get_index_by_NID(name, nid, pos);
		if(pos < 0)
			break; /* end of list */

		if(!ents)
			ents = pcl_vector_create(X509_NAME_entry_count(name),
				sizeof(char**), pcl_vector_cleanup_dblptr);

		str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, pos));
		value = pcl_strndup((const char *)str->data, str->length);
		pcl_vector_append(ents, &value);
	}

	if(!ents)
		SETERR(PCL_ENOTFOUND);

	return ents;
}

int
pcl_ssl_setciphlist(pcl_ssl_t *ssl, const char *ciph_list)
{
	if(strempty(ciph_list))
		return SETERR(PCL_EINVAL);

	if(!SSL_CTX_set_cipher_list(ssl->ctx, ciph_list))
		return SETERRMSG(PCL_ENOTSUP, "Unsupported cipher list: '%s'", ciph_list);

	return 0;
}

int
pcl_ssl_setpeerca(pcl_ssl_t *ssl, const char *path, int peer_depth)
{
	if(!ssl || !ssl->ctx || strempty(path))
		return SETERR(PCL_EINVAL);

	ERR_clear_error();

	/* To verify the client, server must have a list of CA "names", not
	 * complete certificates, to send to client as a certificate request.
	 *
	 * Look at NOTES section here:
	 *
	 *   http://www.openssl.org/docs/ssl/SSL_CTX_set_client_CA_list.html
	 *
	 * Also peek at SSL_CTX_load_verify_locations (NOTES section):
	 *
	 *   http://www.openssl.org/docs/ssl/SSL_CTX_load_verify_locations.html
	 *
	 * which states "In server mode, when requesting a client certificate,
	 * the server must send the list of CAs of which it will accept client
	 * certificates. This list is not influenced by the contents of CAfile
	 * or CApath and must explicitly be set using the
	 * SSL_CTX_set_client_CA_list(3) family of functions."
	 */
	if(ssl->flags & (PCL_SSL_PASSIVE | PCL_SSL_SERVER))
	{
		STACK_OF(X509_NAME) *cert_names = SSL_load_client_CA_file(path);

		if(!cert_names && sk_X509_NAME_num(cert_names) == 0)
			return SETERRMSG(PCL_ENOTFOUND, "No CA certs found at '%s', %s",
				path, sslerror(ssl));

		SSL_CTX_set_client_CA_list(ssl->ctx, cert_names);
	}

	/* Used to verify peer certificate.  Without this on the server, the
	 * client will send its certificate but the server won't verify it.
	 * Above CA_list() stuff only requests the certificate from a client.
	 * Client also needs this call to verify server certificate, however
	 * client doesn't need to load a CA list of names like server.
	 */
	if(SSL_CTX_load_verify_locations(ssl->ctx, path, NULL) == 0)
		return SETERRMSG(PCL_ESSL, "Failed to load peer CA certificate: %s, %s",
			path, sslerror(ssl));

	/* Must enable verify_peer on server-side */
	int server_flags = SSL_VERIFY_PEER | SSL_VERIFY_FAIL_IF_NO_PEER_CERT;

	if(ssl->flags & (PCL_SSL_PASSIVE | PCL_SSL_SERVER))
		SSL_CTX_set_verify(ssl->ctx, server_flags, NULL);
	/* clients use verify_none, this flag ensures we catch verify errors. */
	else
		ssl->flags |= PCL_SSL_VERIFY_PEER;

	SSL_CTX_set_verify_depth(ssl->ctx, max(0,peer_depth));

	return 0;
}

int
pcl_ssl_setpair(pcl_ssl_t *ssl, const char *cert, const char *key)
{
	if(!ssl || !ssl->ctx || strempty(cert) || strempty(key))
		return SETERR(PCL_EINVAL);

	ERR_clear_error();

	if(!SSL_CTX_use_certificate_file(ssl->ctx, cert, SSL_FILETYPE_PEM))
		return SETERRMSG(PCL_ESSL,"Failed to load certificate: '%s', %s",
			cert, sslerror(ssl));

	if(!SSL_CTX_use_PrivateKey_file(ssl->ctx, key, SSL_FILETYPE_PEM))
		return SETERRMSG(PCL_ESSL,"Failed to load private key: '%s', %s",
			key, sslerror(ssl));

	if(!SSL_CTX_check_private_key(ssl->ctx))
		return SETERRMSG(PCL_ESSL, "Invalid cert:key pair: '%s:%s', %s",
			cert, key, sslerror(ssl));

	/* only do this for clients, servers already send their certs with
	 * SSL_VERIFY_NONE.  On clients, this forces cert requests to be sent
	 * by server.
	 */
	if(!(ssl->flags & (PCL_SSL_PASSIVE | PCL_SSL_SERVER)))
		SSL_CTX_set_verify(ssl->ctx, SSL_VERIFY_PEER, NULL);

	return 0;
}

int
pcl_ssl_recv(pcl_ssl_t *ssl, void *buf, size_t len)
{
	pcl_err_clear();
	ERR_clear_error();

	int r = SSL_read(ssl->ssl, buf, (int)len);

	switch(SSL_get_error(ssl->ssl, r))
	{
		case SSL_ERROR_NONE:
			break;

		/* read() returned zero, throw a connreset */
		case SSL_ERROR_ZERO_RETURN:
			pcl_ssl_close(ssl);
			return SETERR(PCL_ECONNRESET);

		case SSL_ERROR_WANT_READ:
			return SETERR(PCL_EAGAIN);

		/* We don't support renegotiations.  Indicate conn aborted. */
		case SSL_ERROR_WANT_WRITE:
			pcl_ssl_close(ssl);
			return SETERRMSG(PCL_ECONNABORTED, "Write request during read", 0);

		/* Consult errno or GetLasterr() */
		case SSL_ERROR_SYSCALL:
		{
			const char *msg;
			int err = pcl_err_os2pcl(pcl_oserrno);

			if(err == PCL_EOKAY || err == PCL_EUNDEF)
				err = PCL_ECONNRESET;

			if((msg = sslerror(ssl)))
				SETERRMSG(err, "%s", msg);
			else
				SETERR(err);

			pcl_ssl_close(ssl);
			return -1;
		}

		default:
			SETERRMSG(PCL_ESSL, "%s", sslerror(ssl));
			pcl_ssl_close(ssl);
			return -1;
	}

	return r;
}

int
pcl_ssl_send(pcl_ssl_t *ssl, const void *data, size_t len)
{
	pcl_err_clear();
	ERR_clear_error();

	int w = SSL_write(ssl->ssl, data, (int)len);

	switch(SSL_get_error(ssl->ssl, w))
	{
		case SSL_ERROR_NONE:
			break;

		/* underlying conn was closed, throw a connreset */
		case SSL_ERROR_ZERO_RETURN:
			return SETERR(PCL_ECONNRESET);

		case SSL_ERROR_WANT_WRITE:
			return SETERR(PCL_EAGAIN);

		/* We don't support renegotiations.  Indicate conn aborted. */
		case SSL_ERROR_WANT_READ:
			pcl_ssl_close(ssl);
			return SETERRMSG(PCL_ECONNABORTED, "Read request during write", 0);

		/* Consult errno or GetLasterr() */
		case SSL_ERROR_SYSCALL:
			SETLASTERRMSG("%s", sslerror(ssl));
			pcl_ssl_close(ssl);
			return -1;

		default:
			SETERRMSG(PCL_ESSL, "%s", sslerror(ssl));
			pcl_ssl_close(ssl);
			return -1;
	}

	return w;
}

bool
pcl_ssl_ispassive(pcl_ssl_t *ssl)
{
	return ssl && ssl->flags & PCL_SSL_PASSIVE;
}

bool
pcl_ssl_isserver(pcl_ssl_t *ssl)
{
	return ssl && ssl->flags & PCL_SSL_SERVER;
}

bool
pcl_ssl_isconnected(pcl_ssl_t *ssl)
{
	return ssl && ssl->ssl && pcl_socket_isalive(ssl->sock);
}

pcl_socket_t *
pcl_ssl_socket(pcl_ssl_t *ssl)
{
	return ssl ? ssl->sock : NULL;
}

pcl_socketfd_t
pcl_ssl_socketfd(pcl_ssl_t *ssl)
{
	return ssl ? pcl_socket_fd(ssl->sock) : INVALID_SOCKET;
}

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

static int
configure_socket(pcl_ssl_t *ssl, const char *host, int port)
{
	int n = 65535;

	if(host)
	{
		ssl->sock = pcl_socket(host, port);
		if(!ssl->sock)
			return TRCMSG("Failed to create socket: host=%s port=%d", host, port);
	}

	if(ssl->flags & PCL_SOCK_NONBLOCK)
		pcl_socket_setnonblocking(ssl->sock, true);

	pcl_socketfd_t fd = pcl_socket_fd(ssl->sock);

	(void)setsockopt(fd, SOL_SOCKET, SO_RCVBUF,
		(const char *)&n, sizeof(int));

	(void)setsockopt(fd, SOL_SOCKET, SO_SNDBUF,
		(const char *)&n, sizeof(int));

	if(ssl->flags & PCL_SSL_PASSIVE)
	{
		n=1;
		(void)setsockopt(fd, SOL_SOCKET, SO_REUSEADDR,
			(const char *)&n, sizeof(int));
	}

	return 0;
}

#if 0
static int
x509_entry_text(X509_NAME_ENTRY *entry, char *out, size_t len)
{
	ASN1_STRING *str = X509_NAME_ENTRY_get_data(entry);

	if(str->length <= (int)len)
		len = str->length;

	memcpy(out, str->data, len);
	out[len] = 0;
	return (int)len;
}
#endif

static int
sslerror_cb(const char *str, size_t _len, void *ssl_err)
{
	pcl_openssl_error_t *err = ssl_err;
	int len = (int)_len, msg_len = err->msg ? (int)strlen(err->msg) : 0;

	/* remove *annoying* LF */
	char *p = strrchr(str, '\n');
	if(p) len = (int)(p - str);

	/* grow when needed, add 5 for " .. \0". NOTE: err->msg is reused for life
	 * of pcl_ssl_t object, so cache msg and size; avoiding future reallocs.
	 */
	size_t size = msg_len + len + 5;
	if(size > err->size)
		err->msg = (char *)pcl_realloc(err->msg, err->size = size);

	/* append 'str' to err->msg, separate using " .. " */
	(void)pcl_sprintf(err->msg + msg_len, err->size - msg_len, "%s%.*s",
		msg_len > 0 ? " .. " : "", len, str);

	return 1; /* 1 to continue error iteration, 0 to stop (from OpenSSL docs) */
}

int
pcl_openssl_error(pcl_openssl_error_t *err)
{
	if(!err)
		return BADARG();

	err->code = ERR_peek_error();

	if(err->msg)
		*err->msg = 0;
	else
		err->size = 0;

	ERR_print_errors_cb(sslerror_cb, err);
	return 0;
}

pcl_openssl_error_t *
pcl_ssl_error(pcl_ssl_t *ssl)
{
	if(!ssl)
		return R_SETERR(NULL, PCL_EINVAL);
	if(pcl_openssl_error(&ssl->err))
		return R_TRC(NULL);
	return &ssl->err;
}

