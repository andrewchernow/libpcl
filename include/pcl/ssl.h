
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

#ifndef LIBPCL_SSL_H
#define LIBPCL_SSL_H

#include <pcl/types.h>
#include <openssl/ossl_typ.h> // X509 structure

/* default cipher suite, also only TLSv1.2+ proto versions are supported */
#define PCL_SSL_DEFCIPHLIST "ECDHE-ECDSA-AES256-GCM-SHA384:" \
"ECDHE-RSA-AES256-GCM-SHA384:ECDHE-ECDSA-CHACHA20-POLY1305:" \
"ECDHE-RSA-CHACHA20-POLY1305:ECDHE-ECDSA-AES128-GCM-SHA256:" \
"ECDHE-RSA-AES128-GCM-SHA256:ECDHE-ECDSA-AES256-SHA384:" \
"ECDHE-RSA-AES256-SHA384:ECDHE-ECDSA-AES128-SHA256:ECDHE-RSA-AES128-SHA256"

#define PCL_SSL_WANTREAD  1  /* connect & handshake, wait for read() */
#define PCL_SSL_WANTWRITE 2  /* connect & handshake, wait for write() */

#define PCL_SSL_SERVER       0x0010 /* Server-side active-socket */
#define PCL_SSL_PASSIVE      0x0020 /* passive-socket (server listener) */
#define PCL_SSL_VERIFY_PEER  0x0040 /* internally set */

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	unsigned long code;
	char *msg;
	size_t size;
} pcl_openssl_error_t;

/* used by pcl_ssl_certentry() */
enum
{
	PCL_SSL_CERT_COUNTRY_NAME,
	PCL_SSL_CERT_LOCALITY_NAME,
	PCL_SSL_CERT_STATE,
	PCL_SSL_CERT_ORG_NAME,
	PCL_SSL_CERT_ORG_UNIT,     /* OU */
	PCL_SSL_CERT_COMMON_NAME   /* CN */
};

/* If PCL_SSL_PASSIVE is set, "..." expects two additional arguments:
 *   const char *listen_host, int listen_port
 * Note: use PCL_SOCK_NONBLOCK from socket API within flags for nonblocking.
 */
PCL_EXPORT pcl_ssl_t *pcl_ssl_create(int flags, ...);

/* closes the connection and does some reseting, but can be reused.
 * use pcl_ssl_free() to completely wipe out an pcl_ssl_t object.
 */
PCL_EXPORT void pcl_ssl_close(pcl_ssl_t *ssl);

PCL_EXPORT void pcl_ssl_free(pcl_ssl_t *ssl);



/* Connects to the server's host and port.  If want_io is provided,
 * it will point to PCL_SSL_WANTREAD or PCL_SSL_WANTWRITE when the SSL socket
 * is non-blocking and the operation would block if continued.
 */
PCL_EXPORT int pcl_ssl_connect(pcl_ssl_t *ssl, const char *host, int port, int *want_io);

PCL_EXPORT pcl_ssl_t *pcl_ssl_accept(pcl_ssl_t *server);

/* Performs the SSL handshake with a client.  If want_io is provided,
 * it will point to PCL_SSL_WANTREAD or PCL_SSL_WANTWRITE when the SSL socket
 * is non-blocking and the operation would block if continued.
 */
PCL_EXPORT int pcl_ssl_handshake(pcl_ssl_t *ssl, int *want_io);

/* Returns the number of bytes actually received.  On error,
 * PCL_EAGAIN is returned for non-blocking sockets that could not
 * make any progress.
 */
PCL_EXPORT int pcl_ssl_recv(pcl_ssl_t *ssl, void *buf, size_t len);

/* Returns the number of bytes actually sent, or -1 on error.  On error,
 * PCL_EAGAIN is returned for non-blocking sockets that could not
 * make any progress.
 */
PCL_EXPORT int pcl_ssl_send(pcl_ssl_t *ssl, const void *data, size_t len);

PCL_EXPORT X509 *pcl_ssl_peercert(pcl_ssl_t *ssl);

/* Gets all certificate entries of the given 'entry_type'.  entry_type is one of PCL_SSL_CERT_xxx
 * enum values.  If no entries are found for the given 'entry_type', NULL is returned and
 * PCL_ENOTFOUND is raised.  If an error occurs, NULL is returned and pcl_errno contains
 * the last error.  On success, a non-NULL vector of char** is returned, use pcl_vec_getptr().
 */
PCL_EXPORT pcl_vector_t *pcl_ssl_certentry(pcl_ssl_t *ssl, int entry_type);

/* Sets the cipher list if something other than PCL_SSL_DEFCIPHLIST is desired.
 * For format of 'ciph_list', see openssl documentation.
 */
PCL_EXPORT int pcl_ssl_setciphlist(pcl_ssl_t *ssl, const char *ciph_list);

/* Sets the peer's CA certificate, so we can verify the peer. */
PCL_EXPORT int pcl_ssl_setpeerca(pcl_ssl_t *ssl, const char *peer_ca_path, int peer_depth);

/* Sets the caller's (either client or server) certificate and
 * RSA private key pair, both expected to be in PEM format.  This allows
 * our peer to verify us.
 */
PCL_EXPORT int pcl_ssl_setpair(pcl_ssl_t *ssl, const char *cert_path, const char *key_path);

PCL_EXPORT bool pcl_ssl_isalive(pcl_ssl_t *ssl);

PCL_EXPORT pcl_socket_t *pcl_ssl_socket(pcl_ssl_t *ssl);

/* This builds an OpenSSL error message and stores it within the given err.
 * Returns 0 on success and -1 on error. Must free err->msg.
 */
PCL_EXPORT int pcl_openssl_error(pcl_openssl_error_t *err);

/* returned msg must be freed */
PCL_INLINE char *
pcl_openssl_errmsg(void)
{
	pcl_openssl_error_t e = {0};
	pcl_openssl_error(&e);
	return e.msg;
}

/* Same as pcl_openssl_error() except this takes an SSL object and returns
 * an pcl_openssl_error_t object that has been populated; internally by
 * pcl_openssl_error(). No need to free anything as the returned error structure is
 * managed by the given ssl object.
 */
PCL_EXPORT pcl_openssl_error_t *pcl_ssl_error(pcl_ssl_t *ssl);

#ifdef __cplusplus
}
#endif
#endif

