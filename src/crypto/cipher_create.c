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

#include "_crypto.h"

pcl_cipher_t *
pcl_cipher_create(const char *algo, const void *key, const void *iv,
	const void *aad, uint32_t aad_len, bool enc)
{
	if(!key || !iv)
		return R_SETERR(NULL, PCL_EINVAL);

	const EVP_CIPHER *cipher = EVP_get_cipherbyname(algo);

	if(!cipher)
		return R_SETERRMSG(NULL, PCL_EINVAL,
			"no such cipher algorithm '%s': see enum pcl_cipher_algo", algo);

	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();
	const char *name = OBJ_nid2ln(EVP_CIPHER_nid(cipher));

	if(!EVP_CipherInit_ex(ctx, cipher, NULL, key, iv, enc))
	{
		EVP_CIPHER_CTX_free(ctx);
		return R_SETERRMSG(NULL, PCL_ECRYPT, "Failed to create %s context: %s",
			enc ? "encryption" : "decryption", name);
	}

	pcl_cipher_t *c = pcl_malloc(sizeof(pcl_cipher_t));

	c->ctx = ctx;
	c->algo = name;

	if(aad && aad_len && pcl_cipher_isaead(c))
	{
		int n;

		/* AAD is set by specifying NULL for output buffer */
		if(!EVP_CipherUpdate(ctx, NULL, &n, aad, aad_len))
		{
			pcl_cipher_free(c);
			return R_SETERRMSG(NULL, PCL_ECRYPT, "Failed to set AAD: %s", name);
		}
	}

	return c;
}
