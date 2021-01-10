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
#include <pcl/array.h>
#include <pcl/error.h>
#include <string.h>

/**$ Append elements to array. */
TESTCASE(array_add)
{
	pcl_array_t *arr = pcl_array(10, NULL);
	const char *elements[] = {
		"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
		"eleven", "tweleve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
		"eighteen", "nineteen", "twenty"
	};

	for(int i = 0; i < (int) (sizeof(elements) / sizeof(elements[0])); i++)
	{
		ASSERT_INTEQ(pcl_array_push(arr, (void *) elements[i]), i + 1, "wrong return value add");
		ASSERT_STREQ(pcl_array_get(arr, i), elements[i], "strings do not match on get");
	}

	pcl_array_free(arr);
	return true;
}

/**$ Get element at an invalid index */
TESTCASE(array_get_badidx)
{
	pcl_array_t *arr = pcl_array(8, NULL);
	ASSERT_NULL(pcl_array_get(arr, 0), "array_get");
	ASSERT_INTEQ(pcl_errno, PCL_EINDEX, "wrong pcl error set");
	pcl_array_free(arr);
	return true;
}

/**$ Get element at a valid index */
TESTCASE(array_get)
{
	pcl_array_t *arr = pcl_array(0, NULL);
	ASSERT_INTEQ(pcl_array_push(arr, "value"), 1, "wrong return value");
	ASSERT_STREQ(pcl_array_get(arr, 0), "value", "wrong array element value");
	pcl_array_free(arr);
	return true;
}

/**$ Get NULL element at a valid index */
TESTCASE(array_get_null)
{
	pcl_array_t *arr = pcl_array(0, NULL);
	ASSERT_INTEQ(pcl_array_push(arr, NULL), 1, "wrong return value");
	ASSERT_NULL(pcl_array_get(arr, 0), "wrong array element value");
	ASSERT_INTEQ(pcl_errno, PCL_EOKAY, "wrong pcl error value");
	pcl_array_free(arr);
	return true;
}

/**$ Set element within an array */
TESTCASE(array_set)
{
	pcl_array_t *arr = pcl_array(4, NULL);
	ASSERT_INTEQ(pcl_array_push(arr, "value1"), 1, "wrong return value array_add");
	ASSERT_INTEQ(pcl_array_push(arr, "value2"), 2, "wrong return value array_add");
	ASSERT_INTEQ(pcl_array_set(arr, "value3", 0), 0, "wrong return value array_set");
	ASSERT_STREQ(pcl_array_get(arr, 0), "value3", "wrong array element value");
	pcl_array_free(arr);
	return true;
}

/**$ Set element beyond current count */
TESTCASE(array_set_beyond_count)
{
	pcl_array_t *arr = pcl_array(2, NULL);
	ASSERT_INTEQ(pcl_array_set(arr, "value", 1), 0, "wrong return value array_set");
	ASSERT_NULL(pcl_array_get(arr, 0), "wrong return value array_get");
	ASSERT_INTEQ(pcl_errno, PCL_EOKAY, "wrong pcl_errno");
	ASSERT_STREQ(pcl_array_get(arr, 1), "value", "wrong array element value");
	pcl_array_free(arr);
	return true;
}

/**$ Remove element from array */
TESTCASE(array_remove)
{
	pcl_array_t *arr = pcl_array(2, NULL);
	ASSERT_INTEQ(pcl_array_push(arr, "value"), 1, "wrong return value array_add");
	ASSERT_INTEQ(pcl_array_remove(arr, 0), 0, "wrong return value array_remove");
	ASSERT_INTEQ(arr->count, 0, "wrong pcl_array_t.count value");
	pcl_array_free(arr);
	return true;
}
