/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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
#include <pcl/stdlib.h>
#include <pcl/stdio.h>
#include <string.h>

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
		err->msg = pcl_realloc(err->msg, err->size = size);

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
