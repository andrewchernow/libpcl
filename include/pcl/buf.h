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

#ifndef LIBPCL_BUF_H
#define LIBPCL_BUF_H

/** @defgroup buf Dynamic Buffer
 * The buffer API provides a dynamically sized buffer for putting text and/or binary data.
 * When a buffer is created, a buffer mode must be provided: one of ::PclBufBinary, ::PclBufText,
 * ::PclBufTextW or ::PclBufTextP. Regardless of a buffer's mode, a trailing NUL is added on
 * every put. The space is reserved internally and it not represented in the buffer's
 * current position or length. The buffer thinks in "characters", not bytes. When in binary
 * mode, it still performs character calculations but based on a one byte character.
 *
 * ### Binary Mode
 * A binary buffer is an array of data types: integers, strings and blobs. A binary string is
 * written to the buffer with a trailing NUL, avoiding the need for a length prefix.
 * Integers are written in network-byte order and blobs are written as is. For blobs, if they
 * are variable length, do ::pcl_buf_putint32 followed by ::pcl_buf_put. There is also
 * int8, int16 and int64.
 *
 * All buffer functions are available to ::PclBufBinary. Again, this mode deals with bytes but
 * treats them as one byte characters.
 *
 * @code
 * pcl_buf_t *buf = pcl_buf_init(NULL, 64, PclBufBinary);
 *
 * pcl_buf_putstr(buf, "Hello"); // buffer contents: "Hello\0\0"
 *
 * // no trailing NUL for single characters
 * pcl_buf_putchar(buf, ' ');    // buffer contents: "Hello\0 \0"
 *
 * // it may appear that the space is joined to World. However, when getting data back
 * // out of the buffer, the idea is to call the same set of functions to deserialize.
 * // Thus, pcl_buf_getchar gets the space and pcl_buf_getstr gets "World"
 * pcl_buf_putstr(buf, "World"); // buffer contents: "Hello\0 World\0\0"
 *
 * // be careful with the raw put function, it won't add a NUL for strings
 * // unless you include the NUL in the length argument.
 * pcl_buf_put(buf, "abc", 3); // No trailing NUL put. Pass 4 or use pcl_buf_putstr
 *
 * // binary mode only, "Hello\0 World\0abc[32 bits in network-byte order]\0"
 * // note the "abc" string without a NUL. pcl_buf_getstr cannot be used, one
 * // must use pcl_buf_getchar 3 times or pcl_buf_get with a length of 3.
 * pcl_buf_putint32(buf, 3897463623);
 *
 * pcl_buf_free(buf);
 * @endcode
 *
 * ### Text Mode
 * In text mode, the buffer is an array of characters with the always present NUL at the end.
 * Text mode buffers are limited to put, putstr, putchar, putf and putvf. Get functions
 * aren't needed in text mode since one can simply access pcl_buf_t.data, which will be a
 * NUL-terminated string. Directly accessing pcl_buf_t.data, means you must properly cast it if
 * using ::PclBufTextW or ::PclBufTextP.
 *
 * @code
 * pcl_buf_t buf; // use stack memory
 *
 * pcl_buf_init(&buf, 64, PclBufText);
 * pcl_buf_putstr(&buf, "Hello"); // buffer contents: "Hello\0"
 * pcl_buf_putstr(&buf, "World"); // buffer contains: "HelloWorld\0"
 * printf("%d] %s\n", buf.len, buf.data); // 10] HelloWorld
 * pcl_buf_clear(&buf); // not pcl_buf_free!
 *
 * // For PclBufTextW (PclBufTextP is identical but use _P("") and pchar_t)
 * pcl_buf_init(&buf, 64, PclBufTextW);
 * pcl_buf_putstr(&buf, L"Hello");
 * printf("%ls\n", (wchar_t *)buf.data);
 * @endcode
 * @{
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

enum pcl_buf_mode
{
	/** Binary buffer mode. Single byte characters and all put and get functions are available. */
	PclBufBinary,

	/** Text buffer mode. This is a \c char buffer with limited access to functions. */
	PclBufText,

	/** Wide-text buffer mode. This is a \c wchar_t buffer with limited acccess to functions. */
	PclBufTextW,

#ifdef PCL_WINDOWS
	PclBufTextP=PclBufTextW
#else
	/** portable-text buffer mode. This is a \c pchar_t buffer with limited acccess to functions. */
	PclBufTextP=PclBufText
#endif
};

struct tag_pcl_buf
{
	/** buffer mode, see ::pcl_buf_mode */
	enum pcl_buf_mode mode;

	/** Size of a character for this buffer in bytes.
	 * Do not change this. This is always 1 for binary mode.
	 */
	int chrsize;

	/** current character position */
	int pos;

	/** number of characters in the buffer */
	int len;

	/** number of allocated characters (capacity) */
	size_t size;

	/** buffer bytes, always `chrsize * size` wide. */
	char *data;
};

/** Initializes a buffer object.
 * @param b pointer to a buffer. If this is \c NULL, a buffer will be allocated
 * @param size intial size in characters
 * @param mode the buffer mode
 * @return pointer to \a b if it was not \c NULL or a pointer to a newly allocated buffer.
 */
PCL_EXPORT pcl_buf_t *pcl_buf_init(pcl_buf_t *b, size_t size, enum pcl_buf_mode mode);

/** Grow a buffer. Given a \a len, this ensures that writing \a len bytes won't overflow
 * the buffer by reallocating the buffer's internal memory. This is used by all buffer put
 * functions before attempting to write data.
 * @param b pointer to a buffer
 * @param len number of new characters to be added to the buffer
 * @return pointer to \a b or \c NULL on error
 */
PCL_EXPORT pcl_buf_t *pcl_buf_grow(pcl_buf_t *b, int len);

/** Reset a buffer to its initial state. This sets the buffer's position and length to zero.
 * @param b pointer to a buffer
 * @return pointer to the \a b argument
 */
PCL_EXPORT pcl_buf_t *pcl_buf_reset(pcl_buf_t *b);

/** Clear a buffer. This is similar to a reset but also releases all resources used by
 * the buffer.
 * @param b pointer to a buffer
 * @return pointer to the \a b argument
 */
PCL_EXPORT pcl_buf_t *pcl_buf_clear(pcl_buf_t *b);

/** Copy a buffer.
 * @param dest pointer to a buffer or \c NULL in which case this is allocated.
 * @param src pointer to the buffer to copy
 * @return pointer to \a dest or, if \a dest is \c NULL, a newly allocated buffer
 */
PCL_EXPORT pcl_buf_t *pcl_buf_copy(pcl_buf_t *dest, const pcl_buf_t *src);

/** Free all buffer resources and the buffer object itself. Do not pass stack memory to this
 * function. Instead, use ::pcl_buf_clear.
 * @param b pointer to a buffer
 * @return always NULL
 */
PCL_EXPORT pcl_buf_t *pcl_buf_free(pcl_buf_t *b);

/** Put a blob into a buffer.
 * @param b pointer to a buffer
 * @param data pointer to the data to write to the buffer
 * @param len number of characters to put
 * @return number of characters written or -1 on error
 */
PCL_EXPORT int pcl_buf_put(pcl_buf_t *b, const void *data, int len);

/** Put a string into a buffer.
 * @param b pointer to a buffer
 * @param s pointer to a string
 * @return number of characters written (including NUL if in binary mode) or -1 on error
 */
PCL_EXPORT int pcl_buf_putstr(pcl_buf_t *b, const void *s);

/** Put a character into a buffer. No NUL is added when putting a char.
 * @param b pointer to a buffer
 * @param c character
 * @return number of bytes written or -1 on error
 */
PCL_EXPORT int pcl_buf_putchar(pcl_buf_t *b, uint32_t c);

/** Put an 8-bit integer into a buffer.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param i 8-bit integer to put
 * @return number of bytes written or -1 on error
 */
PCL_EXPORT int pcl_buf_putint8(pcl_buf_t *b, uint8_t i);

/** Puts a 16-bit integer into a buffer in network-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param i 16-bit integer to put
 * @return number of bytes written or -1 on error
 */
PCL_EXPORT int pcl_buf_putint16(pcl_buf_t *b, uint16_t i);

/** Puts a 32-bit integer into a buffer in network-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param i 32-bit integer to put
 * @return number of bytes written or -1 on error
 */
PCL_EXPORT int pcl_buf_putint32(pcl_buf_t *b, uint32_t i);

/** Puts a 64-bit integer into a buffer in network-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param i 64-bit integer to put
 * @return number of bytes written or -1 on error
 */
PCL_EXPORT int pcl_buf_putint64(pcl_buf_t *b, uint64_t i);

/** Put a formatted string into a buffer.
 * @param b pointer to a buffer
 * @param format PCL format string. The string must match the buffer mode. PclBufText uses
 * \c char*, PclBufTextW uses \c wchar_t* and PclBufTextP uses \c pchar_t* .
 * @param ... variable arguments
 * @return number of characters written (including NUL if in binary mode) or -1 on error
 */
PCL_EXPORT int pcl_buf_putf(pcl_buf_t *b, const void *format, ...);

/** Put a formatted string into a buffer.
 * @param b pointer to a buffer
 * @param format PCL format string. The string must match the buffer mode. PclBufText uses
 * \c char*, PclBufTextW uses \c wchar_t* and PclBufTextP uses \c pchar_t* .
 * @param ap variable arguments
 * @return number of characters written (including NUL if in binary mode) or -1 on error
 */
PCL_EXPORT int pcl_buf_vputf(pcl_buf_t *b, const void *format, va_list ap);

/** Get a blob from a buffer.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] buf a buffer used to store the result
 * @param len length in bytes of \a buf if in binary mode, otherwise character length
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_get(pcl_buf_t *b, void *buf, int len);

/** Get a string from a buffer.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] buf a buffer used to store the result
 * @param len length in bytes of \a buf
 * @return number of bytes read (including NUL if in binary mode) or -1 on error
 */
PCL_EXPORT int pcl_buf_getstr(pcl_buf_t *b, char *buf, int len);

/** Get a character from a buffer.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] c pointer to a character
 * @return number of bytes read  or -1 on error
 */
PCL_EXPORT int pcl_buf_getchar(pcl_buf_t *b, uint32_t *c);

/** Get an 8-bit integer a the buffer.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] i pointer to a 8-bit integer
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint8(pcl_buf_t *b, uint8_t *i);

/** Get a 16-bit integer from a buffer in host-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] i pointer to a 16-bit integer
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint16(pcl_buf_t *b, uint16_t *i);

/** Get a 32-bit integer from a buffer in host-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] i pointer to a 32-bit integer
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint32(pcl_buf_t *b, uint32_t *i);

/** Get a 64-bit integer from a buffer in host-byte order.
 * @note PclBufBinary mode only
 * @param b pointer to a buffer
 * @param[out] i pointer to a 64bit integer
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint64(pcl_buf_t *b, uint64_t *i);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
