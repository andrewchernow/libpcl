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

#ifndef LIBPCL_CRYPTO_H
#define LIBPCL_CRYPTO_H

/** @defgroup crypto Cryptography Functions
 * A set of functions focused on cryptography.
 * @{
 */
#include <pcl/types.h>

#define PCL_MAXIVLEN 16
#define PCL_AEAD_TAGLEN 16

#ifdef __cplusplus
extern "C" {
#endif

/** Generate random bytes.
	* @param buf pointer to a buffer to receive random bytes
	* @param num number of bytes to generate
	* @return number of bytes generated
	*/
PCL_EXPORT int pcl_rand(void *buf, int num);

/** Create a cipher object.
 * @param algo cipher algorithm: supports "aes-128-gcm", "aes-256-gcm", "aes-128-cbc",
 * "aes-256-cbc", "aes-128-ocb", "aes-256-ocb", "chacha20-poly1305".
 * @param key cipher key. length expected to match algorithm
 * @param iv cipher iv. The length depends on the cipher and mode. It is recommended to always
 * provide 16 bytes to keep things simple, which is the maximum IV length across ciphers.
 * @param aad additional authenticated data. This is only used by ciphers that support
 * authenticated encryption with associated data (AEAD). This can be \c NULL in either case.
 * @param aad_len number of \a aad bytes. If zero, \a aad parameter is ignored.
 * @param enc when non-zero, create an encryption cipher, otherwise decryption
 * @return pointer to a cipher object or \c NULL on error
 * @see PCL_MAXIVLEN, enum pcl_cipher_algo
 */
PCL_EXPORT pcl_cipher_t *pcl_cipher(const char *algo, const void *key,
	const void *iv, const void *aad, uint32_t aad_len, bool enc);

/** Reset a cipher so it can be reused.
 * This should be called after calling ::pcl_cipher_final.
 *
 * This requires providing a new IV since using the same IV across cycles is a very bad idea.
 * The key cannot be changed. If the key needs to be changed, free this cipher and create a new
 * one.
 *
 * The goal of reset is to avoid tearing everything down and creating a whole new context when it
 * is desired to reuse the same KEY: set of files, network packets, etc. Instead, it can be reset
 * with a new IV and optional AAD data. After this call, use ::pcl_cipher_update and
 * ::pcl_cipher_final as usual.
 *
 * @param ciph pointer to a cipher object
 * @param iv
 * @param aad additional authenticated data. This is only used by ciphers that support
 * authenticated encryption with associated data (AEAD). This can be \c NULL in either case.
 * @param aad_len number of \a aad bytes. If zero, \a aad parameter is ignored.
 * @return 0 for success and -1 on error.
 */
PCL_EXPORT int pcl_cipher_reset(pcl_cipher_t *ciph, const void *iv,
	const void *aad, uint32_t aad_len);

/** Get cipher name.
 * @param ciph pointer to a cipher object
 * @return pointer to cipher name or \c NULL on error
 */
PCL_EXPORT const char *pcl_cipher_name(pcl_cipher_t *ciph);

/** I this an AEAD cipher.
 * @param ciph pointer to a cipher
 * @return true if AEAD supported, false if not
 */
PCL_EXPORT bool pcl_cipher_isaead(pcl_cipher_t *ciph);

/** Encrypt or dercypt more bytes.
 * @param ciph pointer to a cipher object
 * @param out pointer to an output buffer
 * @param in pointer to an input (plain text)
 * @param in_len number of bytes within \a in
 * @return number of bytes written to \a out or -1 on error
 */
PCL_EXPORT int pcl_cipher_update(pcl_cipher_t *ciph, char *out, const void *in, int in_len);

/** Finalize encrypt or decrypt process.
 * @param ciph pointer to a cipher object
 * @param out pointer to output buffer which must be at least the size of a single block.
 * @param tag Only used for AEAD ciphers. When encrypting, this must be a buffer of at least
 * ::PCL_AEAD_TAGLEN bytes. When decrypting, this is the tag value which must be
 * ::PCL_AEAD_TAGLEN bytes. This can be \c NULL.
 * @return number of bytes written to out (zero is valid) or -1 on error. When using AEAD during
 * decryption, this will fail if the "expected" \a tag value is incorrect. In this case, the
 * plaintext is not trustworthy and should be discarded.
 * @see PCL_AEAD_TAGLEN
 */
PCL_EXPORT int pcl_cipher_final(pcl_cipher_t *ciph, char *out, char *tag);

/** Release all resources used by a cipher object.
 * @param ciph pointer to a cipher object.
 * @see pcl_cipher_reset
 */
PCL_EXPORT void pcl_cipher_free(pcl_cipher_t *ciph);

/** Create a message digest object.
 * @param algo message digest algorithm: supports "md5", "sha1", "sha256", "sha512"
 * @return pointer to a message digest object
 */
PCL_EXPORT pcl_digest_t *pcl_digest(const char *algo);

/** Update the data of a digest operation.
 * @param d pointer to a message digest object
 * @param data pointer to data bytes
 * @param len number of \a data bytes
 * @return 0 for success and -1 on error
 */
PCL_EXPORT int pcl_digest_update(pcl_digest_t *d, const void *data, size_t len);

/** Finalize a digest operation. After this call, the digest object can be reused by calling
 * ::pcl_digest_update and ::pcl_digest_final again.
 * @param d pointer to a message digest object
 * @param[out] lenp if not \c NULL, the digest length will be written here
 * @return pointer to the message digest
 */
PCL_EXPORT void *pcl_digest_final(pcl_digest_t *d, int *lenp);

/** Create a message digest.
 * @param algo message digest algorithm: supports "md5", "sha1", "sha256", "sha512"
 * @param data pointer to data bytes
 * @param[in,out] len on input, this is the number of \a data bytes. On output, this is
 * the number of bytes written to the returned digest.
 * @return pointer to a digest or \c NULL on error
 */
PCL_EXPORT void *pcl_digest_value(const char *algo, const void *data, size_t *len);

/** Get digest length.
 * @param d pointer to a message digest object
 * @return digest length or -1 on error
 */
PCL_EXPORT int pcl_digest_len(pcl_digest_t *d);

/** Get digest algorithm name.
 * @param d pointer to a message digest object
 * @return pointer to digest algorithm name
 */
PCL_EXPORT const char *pcl_digest_name(pcl_digest_t *d);

/** Free a message digest.
 * @param d pointer to a message digest object
 * @return always returns \c NULL
 */
PCL_EXPORT void *pcl_digest_free(pcl_digest_t *d);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_CRYPTO_H
