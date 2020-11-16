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

#ifndef LIBPCL_BUF_H
#define LIBPCL_BUF_H

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ------------------------------------------------------------------------------
 * Management functions
 */

/**
 *
 * @param b
 * @param size
 * @param omit_nul if non-zero, putting strs (putstr, putf, vputf) will not append a NUL. This
 * allows for using the buffer as a dynamic string rather than an array of data types. When zero,
 * the buffer writes NUL bytes for all strings: "abc\0def\0\0" (trailing NUL byte always gets
 * written). With a non-zero value: "abcdef\0". Example represents two calls to putstr. When
 * non-zero, the only allowed GET calls are pcl_buf_get(), pcl_buf_getchar() and pcl_buf_getint8().
 * @return
 */
PCL_EXPORT pcl_buf_t *pcl_buf_init(pcl_buf_t *b, size_t size, bool omit_nul);

/* grow the buffer if writing len bytes would cause an overflow */
PCL_EXPORT pcl_buf_t *pcl_buf_grow(pcl_buf_t *b, int len);

/* Zeros buffer, *b->data set to NUL, pos and len set 0, size not tampered with. */
PCL_EXPORT pcl_buf_t *pcl_buf_reset(pcl_buf_t *b);

/* zeros & releases all resources except the buffer object itself. */
PCL_EXPORT pcl_buf_t *pcl_buf_clear(pcl_buf_t *b);

PCL_EXPORT pcl_buf_t *pcl_buf_copy(pcl_buf_t *dest, pcl_buf_t *src);

/** Gets the current read/write position or sets the position of the buffer.
 * @param b buffer pointer
 * @param[in] newpos new position for the buffer, or NULL to retreive the current position.
 * @return If newpos is NULL, this returns the current position. If newpos is provided,
 * this returns the previous position. If an error occurs, -1 is returned.
 */
PCL_EXPORT int pcl_buf_pos(pcl_buf_t *b, int *newpos);

/** Gets the raw data pointer of the buffer. This can also be used to retreive the buffer length.
 * @param b buffer pointer
 * @param lenp If not NULL, this will receive the length of data.
 * @return pointer to the data or NULL on error.
 */
PCL_EXPORT char *pcl_buf_data(pcl_buf_t *b, int *lenp);

/* only call if buffer was allocated by pcl_buf_init. If not, just call
 * pcl_buf_clear.
 */
PCL_EXPORT pcl_buf_t *pcl_buf_free(pcl_buf_t *b);


/* ------------------------------------------------------------------------------
 * Put functions
 */

PCL_EXPORT int pcl_buf_put(pcl_buf_t *b, const void *data, int len);

/* put a string.
 */
PCL_EXPORT int pcl_buf_putstr(pcl_buf_t *b, const char *s);

#define pcl_buf_putchar(b, c) pcl_buf_putint8(b, (int8_t)(c))

PCL_INLINE int
pcl_buf_putint8(pcl_buf_t *b, uint8_t i)
{
	return pcl_buf_put(b, &i, sizeof(uint8_t));
}

/** Puts a 16-bit integer into the buffer in network-byte order.
 * @param b
 * @param i
 * @return
 */
PCL_EXPORT int pcl_buf_putint16(pcl_buf_t *b, uint16_t i);

/** Puts a 32-bit integer into the buffer in network-byte order.
 * @param b
 * @param i
 * @return
 */
PCL_EXPORT int pcl_buf_putint32(pcl_buf_t *b, uint32_t i);

/** Puts a 64-bit integer into the buffer in network-byte order.
 * @param b
 * @param i
 * @return
 */
PCL_EXPORT int pcl_buf_putint64(pcl_buf_t *b, uint64_t i);

PCL_EXPORT int pcl_buf_putf(pcl_buf_t *b, const char *format, ...);

PCL_EXPORT int pcl_buf_vputf(pcl_buf_t *b, const char *format, va_list ap);


/* ------------------------------------------------------------------------------
 * Get functions
 */

PCL_EXPORT int pcl_buf_get(pcl_buf_t *b, void *buf, int len);

PCL_EXPORT int pcl_buf_getstr(pcl_buf_t *b, char *buf, int len);

#define pcl_buf_getchar(b, chp) pcl_buf_getint8(b, (uint8_t *)(chp))

/** Gets an 8-bit integer from the buffer.
 * @param b
 * @param i
 * @return number of bytes read or -1 on error
 */
PCL_INLINE int
pcl_buf_getint8(pcl_buf_t *b, uint8_t *i)
{
	return pcl_buf_get(b, i, sizeof(uint8_t));
}

/** Gets a 16-bit integer from the buffer in host-byte order.
 * @param b
 * @param i
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint16(pcl_buf_t *b, uint16_t *i);

/** Gets a 32-bit integer from the buffer in host-byte order.
 * @param b
 * @param i
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint32(pcl_buf_t *b, uint32_t *i);

/** Gets a 64-bit integer from the buffer in host-byte order.
 * @param b
 * @param i
 * @return number of bytes read or -1 on error
 */
PCL_EXPORT int pcl_buf_getint64(pcl_buf_t *b, uint64_t *i);

#ifdef __cplusplus
}
#endif
#endif
