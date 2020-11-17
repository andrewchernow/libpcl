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
#include <openssl/err.h>

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
