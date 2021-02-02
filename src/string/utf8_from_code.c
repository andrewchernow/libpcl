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

int
pcl_utf8_from_code(uint32_t code, char *utf8buf)
{
	if(!utf8buf)
		return BADARG();

	if(code <= 0x7F) /* ascii */
	{
		*utf8buf = (char) code;
		return 1;
	}

	if(code <= 0x07FF) /* 2-byte sequence */
	{
		*utf8buf++ = (char) (((code >> 6) & 0x1F) | 0xC0);
		*utf8buf = (char) (((code >> 0) & 0x3F) | 0x80);
		return 2;
	}

	if(code <= 0xFFFF) /* 3-byte sequence */
	{
		*utf8buf++ = (char) (((code >> 12) & 0x0F) | 0xE0);
		*utf8buf++ = (char) (((code >> 6) & 0x3F) | 0x80);
		*utf8buf = (char) (((code >> 0) & 0x3F) | 0x80);
		return 3;
	}

	if(code <= 0x10FFFF) /* 4-byte sequence */
	{
		*utf8buf++ = (char) (((code >> 18) & 0x07) | 0xF0);
		*utf8buf++ = (char) (((code >> 12) & 0x3F) | 0x80);
		*utf8buf++ = (char) (((code >> 6) & 0x3F) | 0x80);
		*utf8buf = (char) (((code >> 0) & 0x3F) | 0x80);
		return 4;
	}

	return SETERRMSG(PCL_EINVAL, "invalid codepoint %08x\n", code);
}
