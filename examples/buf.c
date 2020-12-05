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
#include <pcl/buf.h>
#include <pcl/defs.h>
#include <pcl/error.h>
#include <pcl/string.h>

int main(int argc, char **argv)
{
	UNUSED(argc || argv);

	pcl_init();

	/* binary buffer, great for serializing binary network packets */
	pcl_buf_t *buf = pcl_buf_init(NULL, 128, PclBufBinary);

	const char blob[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};

	pcl_buf_putint8(buf, 100);
	pcl_buf_putint16(buf, 42000);
	pcl_buf_putint32(buf, 3000000000);
	pcl_buf_put(buf, blob, 16);
	pcl_buf_putint64(buf, 10000000000);
	pcl_buf_putstr(buf, "string");
	pcl_buf_putf(buf, "another %s", "string");

	uint8_t i8;
	uint16_t i16;
	uint32_t i32;
	uint64_t i64;
	char s1[32], s2[32];
	char blob_buf[16];
	/* set position to beginning of buffer */
	buf->pos = 0;

	pcl_buf_getint8(buf, &i8);
	pcl_buf_getint16(buf, &i16);
	pcl_buf_getint32(buf, &i32);
	pcl_buf_get(buf, blob_buf, 16);
	pcl_buf_getint64(buf, &i64);

	if(pcl_buf_getstr(buf, s1, (int) sizeof(s1)) < 0)
		PTRACE(NULL, 0);

	if(pcl_buf_getstr(buf, s2, (int) sizeof(s2)) < 0)
		PTRACE(NULL, 0);

	int n=0;

	if(i8 != 100)
		n += printf("int8 didn't match\n");

	if(i16 != 42000)
		n += printf("int16 didn't match\n");

	if(i32 != 3000000000)
		n += printf("int32 didn't match\n");

	if(memcmp(blob, blob_buf, 16))
		n += printf("blobs didn't match\n");

	if(i64 != 10000000000)
		n += printf("int64 didn't match\n");

	if(strcmp(s1, "string"))
		n += printf("putstr didn't match\n");

	if(strcmp(s2, "another string"))
		n += printf("putf didn't match\n");

	if(n == 0)
		printf("PclBufBinary success\n");

	pcl_buf_free(buf);

	/* string buffer */
	buf = pcl_buf_init(NULL, 128, PclBufTextW);

	pcl_buf_putstr(buf, L"string");
	pcl_buf_putchar(buf, L' ');
	pcl_buf_putf(buf, L"another %ls", L"string");

	if(wcscmp((wchar_t *) buf->data, L"string another string"))
		printf("PclBufTextW failed\n");
	else
		printf("PclBufTextW success\n");

	return 0;
}
