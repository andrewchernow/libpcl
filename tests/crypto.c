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

#include "test.h"
#include <pcl/crypto.h>
#include <pcl/string.h>
#include <pcl/alloc.h>
#include <stdio.h>

#define MD5DIGEST "1d4e913914f6f8697fa5458a07aff497"
#define SHA1DIGEST "bac803da916745156689eb83c0c9edb940d635d3"
#define SHA256DIGEST "771cf3d0c3bfb5f3a26e7e26b7b225b497372bd162f978c144c5e56178ae3365"
#define SHA512DIGEST "7f28eae13cbfa79d5ad79b14001f7b94704ab9e98dc0de48125e51c845071b903c8e792e56d231a945b721e7e853abfe21ce3cf1e3f14733549981c8aff70f05"

#define PLAINTEXT \
"We the People of the United States, in Order to form\n" \
"a more perfect Union, establish Justice, insure domestic\n" \
"Tranquility, provide for the common defence, promote the\n" \
"general Welfare, and secure the Blessings of Liberty to\n" \
"ourselves and our Posterity, do ordain and establish this\n" \
"Constitution for the United States of America.\n"

static bool
validate_cipher(const char *algo, const char *aad, int aad_len)
{
	char key[32], iv[PCL_MAXIVLEN], tag[PCL_AEAD_TAGLEN];
	char ciphtext[sizeof(PLAINTEXT) + 32];
	int plaintext_len = sizeof(PLAINTEXT) - 1;
	int ciphtext_len, r;
	pcl_cipher_t *ciph = NULL;
	char msgbuf[512];

	pcl_rand(key, 32);

	/* do encrypt part twice to test pcl_cipher_reset. */
	for(int i = 0; i < 2; i++)
	{
		ciphtext_len = 0;

		/* reset requires a new IV */
		pcl_rand(iv, PCL_MAXIVLEN);

		if(i == 0)
		{
			ciph = pcl_cipher_create(algo, key, iv, aad, aad_len, true);
			sprintf(msgbuf, "Failed to create encrypt cipher %s", algo);
			ASSERT_NOTNULL(ciph, msgbuf);
		}
		else
		{
			/* reuse key is the idea but must change IV */
			sprintf(msgbuf, "Failed to reset encrypt cipher %s", algo);
			ASSERT_INTEQ(pcl_cipher_reset(ciph, iv, aad, aad_len), 0, msgbuf);
		}

		r = pcl_cipher_update(ciph, ciphtext, PLAINTEXT, plaintext_len);
		sprintf(msgbuf, "Failed to update encrypt cipher %s", algo);
		ASSERT_INTNEQ(r, -1, msgbuf);

		ciphtext_len += r;

		/* On encrypt, this gets the "tag" generated */
		r = pcl_cipher_final(ciph, ciphtext + ciphtext_len, tag);
		sprintf(msgbuf, "Failed to finalize encrypt cipher %s", algo);
		ASSERT_INTNEQ(r, -1, msgbuf);

		ciphtext_len += r;
	}

	pcl_cipher_free(ciph);

	// ---------------------------------------------------------
	// decrypt

	int dectext_len = 0;
	char dectext[sizeof(ciphtext)];

	ciph = pcl_cipher_create(algo, key, iv, aad, aad_len, false);
	sprintf(msgbuf, "Failed to create decrypt cipher %s", algo);
	ASSERT_NOTNULL(ciph, msgbuf);

	r = pcl_cipher_update(ciph, dectext, ciphtext, ciphtext_len);
	sprintf(msgbuf, "Failed to update decrypt cipher %s", algo);
	ASSERT_INTNEQ(r, -1, msgbuf);

	dectext_len += r;

	/* On decrypt, this sets the "tag" for comparison */
	r = pcl_cipher_final(ciph, dectext + dectext_len, tag);
	sprintf(msgbuf, "Failed to finalize decrypt cipher %s", algo);
	ASSERT_INTNEQ(r, -1, msgbuf);

	dectext_len += r;
	dectext[dectext_len] = 0;

	sprintf(msgbuf, "descrypt text does match inital text cipher %s", algo);
	ASSERT_STREQ(dectext, PLAINTEXT, msgbuf);

	pcl_cipher_free(ciph);
	return true;
}

/**$ Encrypt and decrypt some text using all supported ciphers. */
TESTCASE(cyrpto_cipher)
{
	char *aad = "additional authenticated data";
	size_t aad_len = strlen(aad);
	const char *alorithms[] = {
		"aes-128-gcm", "aes-256-gcm", "aes-128-ocb", "aes-256-ocb",
		"aes-128-cbc", "aes-256-cbc", "chacha20-poly1305"
	};

	/* the crypto module ignores AAD unless pcl_cipher_isaead() is true. */
	for(int i = 0; i < countof(alorithms); i++)
		if(!validate_cipher(alorithms[i], aad, (int) aad_len))
			return false;

	return true;
}

/**$ Generate a digest for all supported digest alorithms */
TESTCASE(crypto_digest)
{
	char msgbuf[4096];
	pcl_digest_t *md[4];
	int plaintext_len = (int) strlen(PLAINTEXT);

	md[0] = pcl_digest_create("md5");
	md[1] = pcl_digest_create("sha1");
	md[2] = pcl_digest_create("sha256");
	md[3] = pcl_digest_create("sha512");

	for(int i = 0; i < countof(md); i++)
	{
		sprintf(msgbuf, "Failed to update digest %s", pcl_digest_name(md[i]));
		ASSERT_INTNEQ(pcl_digest_update(md[i], PLAINTEXT, plaintext_len), -1, msgbuf);
	}

	for(int i = 0; i < countof(md); i++)
	{
		int len;
		char hex[200];
		void *digest = pcl_digest_final(md[i], &len);
		const char *name = pcl_digest_name(md[i]);
		const char *expected;

		sprintf(msgbuf, "Failed to update digest %s", name);
		ASSERT_NOTNULL(digest, msgbuf);

		sprintf(msgbuf, "hex encode digest %s failed", name);
		ASSERT_INTNEQ(pcl_hex_encode(hex, sizeof(hex), digest, len), -1, msgbuf);

		if(strcmp(name, "md5") == 0)
			expected = MD5DIGEST;
		else if(strcmp(name, "sha1") == 0)
			expected = SHA1DIGEST;
		else if(strcmp(name, "sha256") == 0)
			expected = SHA256DIGEST;
		else
			expected = SHA512DIGEST;

		sprintf(msgbuf, "digests do not match for %s", name);
		ASSERT_STREQ(hex, expected, msgbuf);

		pcl_free(digest);
		pcl_digest_free(md[i]);
	}

	return true;
}
