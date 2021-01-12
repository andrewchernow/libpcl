/*
  Portable C Library (PCL)
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

#include "_crypto.h"
#include <string.h>

int
pcl_cipher_reset(pcl_cipher_t *ciph, const void *iv, const void *aad, uint32_t aad_len)
{
	if(!ciph || !iv)
		return BADARG();

	if(!memcmp(EVP_CIPHER_CTX_original_iv(ciph->ctx), iv, EVP_CIPHER_CTX_iv_length(ciph->ctx)))
		return SETERRMSG(PCL_EINVAL, "attempt to reset %s cipher with same IV", ciph->algo);

	/* The IV should always be changed between cycles! We enforce this. However, the key should
	 * never be changed for a single pcl_cipher_t instance. The whole goal is to maintain the
	 * key for a series of operations that requires the same key: a set of files, set of network
	 * packets, etc. We do not allow changing the 'enc' value, thus the -1.
	 */
	if(EVP_CipherInit_ex(ciph->ctx, NULL, NULL, NULL, iv, -1) == 0)
		return SETERRMSG(PCL_ECRYPT, "Failed to reset %s context", ciph->algo);

	if(aad && aad_len && pcl_cipher_isaead(ciph))
	{
		int n;

		if(EVP_CipherUpdate(ciph->ctx, NULL, &n, (const unsigned char *) aad, (int) aad_len) == 0)
			return SETERRMSG(PCL_ECRYPT, "Failed to set AAD: %s", ciph->algo);
	}

	return 0;
}
