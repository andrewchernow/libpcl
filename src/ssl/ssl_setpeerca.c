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
#include <openssl/err.h>

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
