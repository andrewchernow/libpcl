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
#include <pcl/buf.h>
#include <string.h>

/**$ Put data in binary mode */
TESTCASE(buf_bin_put)
{
	const uint8_t data[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_put(b, data, 10), 10, "wrong return value put");
	ASSERT_INTEQ(b->data[10], 0, "buf data missing NUL");
	ASSERT_INTEQ(memcmp(b->data, data, 10), 0, "wrong buf data contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put a string in binary mode */
TESTCASE(buf_bin_putstr)
{
	const char *str = "Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putstr(b, str), 6, "wrong return value putstr");
	ASSERT_INTEQ(b->data[5], 0, "binary string missing NUL");
	ASSERT_INTEQ(b->data[6], 0, "buf data missing NUL");
	ASSERT_STREQ(b->data, str, "wrong buf data contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put a formatted string in binary mode */
TESTCASE(buf_bin_putf)
{
	const char *str = "Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putf(b, "%s", str), 6, "wrong return value putf");
	ASSERT_INTEQ(b->data[5], 0, "binary string missing NUL");
	ASSERT_INTEQ(b->data[6], 0, "buf data missing NUL");
	ASSERT_STREQ(b->data, str, "wrong buf data contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put an int8 in binary mode */
TESTCASE(buf_bin_putint8)
{
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putint8(b, 255), 1, "wrong return value putint8");
	ASSERT_INTEQ(b->data[1], 0, "buf data missing NUL");

	b->pos = 0;

	uint8_t n;
	ASSERT_INTEQ(pcl_buf_getint8(b, &n), 1, "wrong return value getint8");
	ASSERT_INTEQ(n, 255, "wrong value from getint8");
	pcl_buf_free(b);
	return true;
}

/**$ Put an int16 in binary mode */
TESTCASE(buf_bin_putint16)
{
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putint16(b, 42189), 2, "wrong return value putint16");
	ASSERT_INTEQ(b->data[2], 0, "buf data missing NUL");

	b->pos = 0;

	uint16_t n;
	ASSERT_INTEQ(pcl_buf_getint16(b, &n), 2, "wrong return value getint16");
	ASSERT_INTEQ(n, 42189, "wrong value from getint16");
	pcl_buf_free(b);
	return true;
}

/**$ Put an int32 in binary mode */
TESTCASE(buf_bin_putint32)
{
	uint32_t val = 3U * 1024 * 1024 * 1024;
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putint32(b, val), 4, "wrong return value putint32");
	ASSERT_INTEQ(b->data[4], 0, "buf data missing NUL");

	b->pos = 0;

	uint32_t n;
	ASSERT_INTEQ(pcl_buf_getint32(b, &n), 4, "wrong return value getint32");
	ASSERT_INTEQ(n, val, "wrong value from getint32");
	pcl_buf_free(b);
	return true;
}

/**$ Put an int64 in binary mode */
TESTCASE(buf_bin_putint64)
{
	uint64_t val = 3ULL * 1024 * 1024 * 1024 * 1024;
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufBinary);
	ASSERT_INTEQ(pcl_buf_putint64(b, val), 8, "wrong return value putint64");
	ASSERT_INTEQ(b->data[8], 0, "buf data missing NUL");

	b->pos = 0;

	uint64_t n;
	ASSERT_INTEQ(pcl_buf_getint64(b, &n), 8, "wrong return value getint64");
	ASSERT_INTEQ(n, val, "wrong value from getint64");
	pcl_buf_free(b);
	return true;
}

/**$ Put a string in text mode */
TESTCASE(buf_text_putstr)
{
	const char *str = "Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufText);

	b->data[6] = 1; // ensure 2nd NUL is not added like binary mode

	ASSERT_INTEQ(pcl_buf_putstr(b, str), 5, "wrong return value putstr");
	ASSERT_INTEQ(b->data[5], 0, "buf data missing NUL");
	ASSERT_INTEQ(b->data[6], 1, "extra NUL after putstr");
	ASSERT_STREQ(b->data, str, "wrong buf contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put a formatted string in text mode */
TESTCASE(buf_text_putf)
{
	const char *str = "Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufText);

	b->data[6] = 1; // ensure 2nd NUL is not added like binary mode

	ASSERT_INTEQ(pcl_buf_putf(b, "%s", str), 5, "wrong return value putf");
	ASSERT_INTEQ(b->data[5], 0, "buf data missing NUL");
	ASSERT_INTEQ(b->data[6], 1, "extra NUL after putstr");
	ASSERT_STREQ(b->data, str, "wrong buf contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put a character in text mode (same as binary mode) */
TESTCASE(buf_text_putchar)
{
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufText);
	ASSERT_INTEQ(pcl_buf_putchar(b, 'a'), 1, "wrong return value putchar");
	ASSERT_INTEQ(b->data[1], 0, "buf data missing NUL");
	ASSERT_INTEQ(b->data[0], 'a', "wrong buf contents");
	pcl_buf_free(b);
	return true;
}

/**$ Put a string in text mode */
TESTCASE(buf_textw_putstr)
{
	const wchar_t *str = L"Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufTextW);
	wchar_t *data = (wchar_t *) b->data;

	data[6] = 1; // ensure 2nd NUL is not added like binary mode

	ASSERT_INTEQ(pcl_buf_putstr(b, str), 5, "wrong return value putstr");
	ASSERT_INTEQ(data[5], 0, "buf data missing NUL");
	ASSERT_INTEQ(data[6], 1, "extra NUL after putstr");
	ASSERT_WSTREQ(data, str, "wrong buf contents");

	pcl_buf_free(b);
	return true;
}

/**$ Put a formatted string in text mode */
TESTCASE(buf_textw_putf)
{
	const wchar_t *str = L"Hello";
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufTextW);
	wchar_t *data = (wchar_t *) b->data;

	data[6] = 1; // ensure 2nd NUL is not added like binary mode

	ASSERT_INTEQ(pcl_buf_putf(b, L"%ls", str), 5, "wrong return value putf");
	ASSERT_INTEQ(data[5], 0, "buf data missing NUL");
	ASSERT_INTEQ(data[6], 1, "extra NUL after putstr");
	ASSERT_WSTREQ(data, str, "wrong buf contents");

	pcl_buf_free(b);
	return true;
}

/**$ Put a character in text mode (same as binary mode) */
TESTCASE(buf_textw_putchar)
{
	pcl_buf_t *b = pcl_buf_init(NULL, 10, PclBufTextW);
	ASSERT_INTEQ(pcl_buf_putchar(b, L'a'), 1, "wrong return value putchar");

	wchar_t *data = (wchar_t *) b->data;

	ASSERT_INTEQ(data[1], 0, "buf data missing NUL");
	ASSERT_INTEQ(data[0], L'a', "wrong buf contents");

	pcl_buf_free(b);
	return true;
}

