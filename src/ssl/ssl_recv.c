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
