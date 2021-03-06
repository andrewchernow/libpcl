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

#ifndef LIBPCL_TEST_H
#define LIBPCL_TEST_H

#include <pcl/defs.h>
#include <pcl/types.h>

#define TESTCASE(name) PCL_PUBLIC bool testcase_ ## name(void)

#define ASSERT_INTEQ(actual, expected, message) do{ \
  if(!assert_int_equal(PCL_LOCATION_ARGS, actual, expected, message)) \
    return false; \
}while(0)

#define ASSERT_DOUBLEEQ(actual, expected, message) do{ \
  if(!assert_double_equal(PCL_LOCATION_ARGS, actual, expected, message)) \
    return false; \
}while(0)

#define ASSERT_INTNEQ(actual, expected, message) do{ \
  if(!assert_int_notequal(PCL_LOCATION_ARGS, actual, expected, message)) \
    return false; \
}while(0)

#define ASSERT_STREQ(actual, expected, message) do{ \
  if(!assert_str_equal(PCL_LOCATION_ARGS, actual, expected, message)) \
    return false; \
}while(0)

#define ASSERT_WSTREQ(actual, expected, message) do{ \
  if(!assert_wstr_equal(PCL_LOCATION_ARGS, actual, expected, message)) \
    return false; \
}while(0)

#define ASSERT_NULL(ptr, message) do{ \
  if(!assert_null(PCL_LOCATION_ARGS, ptr, message)) \
    return false; \
}while(0)

#define ASSERT_NOTNULL(ptr, message) do{ \
  if(!assert_notnull(PCL_LOCATION_ARGS, ptr, message)) \
    return false; \
}while(0)

#define ASSERT_TRUE(value, message) do{ \
  int __b = (value) ? 1 : 0; \
  if(!assert_int_equal(PCL_LOCATION_ARGS, __b, 1, message)) \
    return false; \
}while(0)

#define ASSERT_FALSE(value, message) do{ \
  int __bool = (value) ? 1 : 0; \
  if(!assert_int_equal(PCL_LOCATION_ARGS, __bool, 0, message)) \
    return false; \
}while(0)

#ifdef __cplusplus
extern "C" {
#endif

bool
assert_int_equal(PCL_LOCATION_PARAMS, long long actual, long long expected, const char *message);

bool
assert_double_equal(PCL_LOCATION_PARAMS, double actual, double expected, const char *message);

bool
assert_int_notequal(PCL_LOCATION_PARAMS, long long actual, long long expected, const char *message);

bool assert_str_equal(PCL_LOCATION_PARAMS, const char *actual, const char *expected,
	const char *message);

bool assert_wstr_equal(PCL_LOCATION_PARAMS, const wchar_t *actual, const wchar_t *expected,
	const char *message);

bool assert_null(PCL_LOCATION_PARAMS, const void *ptr, const char *message);

bool assert_notnull(PCL_LOCATION_PARAMS, const void *ptr, const char *message);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL_TEST_H
