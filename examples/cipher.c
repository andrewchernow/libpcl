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

#include <pcl/init.h>
#include <pcl/crypto.h>
#include <pcl/string.h>
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <stdlib.h>

#define PLAINTEXT \
"We the People of the United States, in Order to form\n" \
"a more perfect Union, establish Justice, insure domestic\n" \
"Tranquility, provide for the common defence, promote the\n" \
"general Welfare, and secure the Blessings of Liberty to\n" \
"ourselves and our Posterity, do ordain and establish this\n" \
"Constitution for the United States of America.\n"

static void validate_cipher(const char *algo, const char *aad, int aad_len);

int pcl_main(int argc, pchar_t **argv)
{
	pcl_init();

	size_t aad_len = 0;
	char *aad = argc > 1 ? pcl_pcs_to_utf8(argv[1], 0, &aad_len) : NULL;
	const char *alorithms[] = {
		"aes-128-gcm", "aes-256-gcm", "aes-128-ocb", "aes-256-ocb",
		"aes-128-cbc", "aes-256-cbc", "chacha20-poly1305"
	};

	for(int i = 0; i < countof(alorithms); i++)
		validate_cipher(alorithms[i], aad, (int) aad_len);

	pcl_free_safe(aad);

	return 0;
}

static void
validate_cipher(const char *algo, const char *aad, int aad_len)
{
	char key[32], iv[PCL_MAXIVLEN], tag[PCL_AEAD_TAGLEN];
	char ciphtext[sizeof(PLAINTEXT) + 32];
	int plaintext_len = sizeof(PLAINTEXT) - 1;
	int ciphtext_len, r;
	pcl_cipher_t *ciph = NULL;

	pcl_rand(key, 32);

	/* do encrypt part twice to demo pcl_cipher_reset. */
	for(int i = 0; i < 2; i++)
	{
		ciphtext_len = 0;

		/* reset requires a new IV */
		pcl_rand(iv, PCL_MAXIVLEN);

		if(i == 0)
		{
			ciph = pcl_cipher(algo, key, iv, aad, aad_len, true);

			if(!ciph)
				PANIC("", 0);
		}
		else
		{
			/* reuse key is the idea but must change IV */
			if(pcl_cipher_reset(ciph, iv, aad, aad_len))
				PANIC("", 0);

			printf("\n%s\n", pcl_cipher_name(ciph));
			printf("------------------------------------------------------------\n");
		}

		r = pcl_cipher_update(ciph, ciphtext, PLAINTEXT, plaintext_len);

		if(r < 0)
			PANIC("", 0);

		ciphtext_len += r;

		/* On encrypt, this gets the "tag" generated */
		r = pcl_cipher_final(ciph, ciphtext + ciphtext_len, tag);

		if(r < 0)
			PANIC(NULL, 0);

		ciphtext_len += r;

		if(i == 1 && pcl_cipher_isaead(ciph))
		{
			char hex[200];
			pcl_hex_encode(hex, sizeof(hex), tag, sizeof(tag));
			printf("Authentication Tag: %s\n", hex);
			printf("Additional Authenticated Data: %s\n", aad && aad_len ? aad : "<not provided>");
		}
	}

	pcl_cipher_free(ciph);

	// ---------------------------------------------------------
	// decrypt

	int dectext_len = 0;
	char dectext[sizeof(ciphtext)];

	ciph = pcl_cipher(algo, key, iv, aad, aad_len, false);

	if(!ciph)
		PANIC("", 0);

	r = pcl_cipher_update(ciph, dectext, ciphtext, ciphtext_len);

	if(r < 0)
		PANIC("", 0);

	dectext_len += r;

	/* On decrypt, this sets the "tag" for comparison */
	r = pcl_cipher_final(ciph, dectext + dectext_len, tag);

	if(r < 0)
		PANIC(NULL, 0);

	dectext_len += r;
	dectext[dectext_len] = 0;

	printf("Decrypted Text: \n%s\n", dectext);

	pcl_cipher_free(ciph);
}
