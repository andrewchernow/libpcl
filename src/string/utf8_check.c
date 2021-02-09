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

#include <pcl/string.h>
#include <pcl/error.h>

/* Table found here:
 * https://lemire.me/blog/2018/05/09/how-quickly-can-you-check-that-a-string-is-valid-unicode-utf-8/
 *
 * Code Points        1st       2s       3s       4s
 * U+0000..U+007F     00..7F
 * U+0080..U+07FF     C2..DF   80..BF
 * U+0800..U+0FFF     E0       A0..BF   80..BF
 * U+1000..U+CFFF     E1..EC   80..BF   80..BF
 * U+D000..U+D7FF     ED       80..9F   80..BF
 * U+E000..U+FFFF     EE..EF   80..BF   80..BF
 * U+10000..U+3FFFF   F0       90..BF   80..BF   80..BF
 * U+40000..U+FFFFF   F1..F3   80..BF   80..BF   80..BF
 * U+100000..U+10FFFF F4       80..8F   80..BF   80..BF
 */

int
pcl_utf8_check(const char *s, size_t len)
{
	if(len == 0)
		len = strlen(s);

	const char *end = s + len;

	while(s < end)
	{
		unsigned char c = (unsigned char) *s;

		/* ascii */
		if(c <= 0x7F)
		{
			s++;
		}
		else if(c >= 0xC2 && c <= 0xDF) /* 2-byte sequence */
		{
			if(s + 1 >= end)
				return SETERRMSG(PCL_EILSEQ, "2-byte sequence missing byte: first=%02hhx", c);

			unsigned char c2 = (unsigned char) *++s;

			if(c2 < 0x80 || c2 > 0xBF)
				return SETERRMSG(PCL_EILSEQ, "2-byte sequence has invalid 2nd byte: "
																		 "first=%02hhx, second=%02hhx", c, c2);

			s++;
		}
		else if(c >= 0xE0 && c <= 0xEF) /* 3-byte sequence */
		{
			if(s + 2 >= end)
				return SETERRMSG(PCL_EILSEQ, "3-byte sequence missing bytes: first=%02hhx", c);

			unsigned char c2 = (unsigned char) *++s;

			if(c == 0xE0)
			{
				if(c2 < 0xA0 || c2 > 0xBF)
					return SETERRMSG(PCL_EILSEQ, "3-byte sequence has invalid 2nd byte: "
																			 "first=%02hhx, second=%02hhx", c, c2);
			}
			else if(c == 0xED)
			{
				if(c2 < 0x80 || c2 > 0x9F)
					return SETERRMSG(PCL_EILSEQ, "3-byte sequence has invalid 2nd byte: "
																			 "first=%02hhx, second=%02hhx", c, c2);
			}
			else if(c2 < 0x80 || c2 > 0xBF)
			{
				return SETERRMSG(PCL_EILSEQ, "3-byte sequence has invalid 2nd byte: "
																		 "first=%02hhx, second=%02hhx", c, c2);
			}

			unsigned char c3 = (unsigned char) *++s;

			if(c3 < 0x80 || c3 > 0xBF)
				return SETERRMSG(PCL_EILSEQ, "3-byte sequence has invalid 3rd byte: "
																		 "first=%02hhx, second=%02hhx, third=%02hhx", c, c2, c3);

			s++;
		}
		else if(c >= 0xF0 && c <= 0xF4) /* 4-byte sequence */
		{
			if(s + 3 >= end)
				return SETERRMSG(PCL_EILSEQ, "4-byte sequence missing bytes: first=%02hhx", c);

			unsigned char c2 = (unsigned char) *++s;

			if(c == 0xF0)
			{
				if(c2 < 0x90 || c2 > 0xBF)
					return SETERRMSG(PCL_EILSEQ, "4-byte sequence has invalid 2nd byte: "
																			 "first=%02hhx, second=%02hhx", c, c2);
			}
			else if(c2 < 0x80 || c > 0xBF)
			{
				return SETERRMSG(PCL_EILSEQ, "4-byte sequence has invalid 2nd byte: "
																		 "first=%02hhx, second=%02hhx", c, c2);
			}

			unsigned char c3 = (unsigned char) *++s;

			if(c3 < 0x80 || c3 > 0xBF)
				return SETERRMSG(PCL_EILSEQ, "4-byte sequence has invalid 3rd byte: "
																		 "first=%02hhx, second=%02hhx, third=%02hhx", c, c2, c3);

			unsigned char c4 = (unsigned char) *++s;

			if(c4 < 0x80 || c4 > 0xBF)
				return SETERRMSG(PCL_EILSEQ, "4-byte sequence has invalid 4th byte: first=%02hhx, "
																		 "second=%02hhx, third=%02hhx, fourth=%02hhx", c, c2, c3, c4);
			s++;
		}
		else
		{
			return SETERRMSG(PCL_EILSEQ, "invalid first byte: %02hxx", c);
		}
	}

	return 0;
}
