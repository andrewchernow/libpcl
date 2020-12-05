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

int
pcl_cipher_final(pcl_cipher_t *ciph, char *out, char *tag)
{
	if(!ciph || !out)
		return BADARG();

	int out_len;
	int enc = EVP_CIPHER_CTX_encrypting(ciph->ctx);

	/* when given a tag on decrypt, using an AEAD cipher, set the tag value */
	if(tag && !enc && pcl_cipher_isaead(ciph))
		if(EVP_CIPHER_CTX_ctrl(ciph->ctx, EVP_CTRL_AEAD_SET_TAG, PCL_AEAD_TAGLEN, tag) == 0)
			return SETERRMSG(PCL_ECRYPT, "Failed to set %s decrypt tag value", ciph->algo);

	if(EVP_CipherFinal_ex(ciph->ctx, (unsigned char *) out, &out_len) == 0)
		return SETERRMSG(PCL_ECRYPT, "Failed to %s %s final block", ciph->algo,
			enc ? "encrypt" : "decrypt");

	/* when given a tag, using an AEAD cipher, get the generated tag value */
	if(tag && enc && pcl_cipher_isaead(ciph))
		if(EVP_CIPHER_CTX_ctrl(ciph->ctx, EVP_CTRL_AEAD_GET_TAG, PCL_AEAD_TAGLEN, tag) == 0)
			return SETERRMSG(PCL_ECRYPT, "Failed to get %s encrypt tag value", ciph->algo);

	return out_len;
}
