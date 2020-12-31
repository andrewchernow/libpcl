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

#define ELEMCOUNT (int) (sizeof(elements) / sizeof(elements[0]))

static const char *elements[] = {
	"one", "two", "three", "four", "five", "six", "seven", "eight", "nine", "ten",
	"eleven", "tweleve", "thirteen", "fourteen", "fifteen", "sixteen", "seventeen",
	"eighteen", "nineteen", "twenty"
};

/**$ Append elements to array. */
TESTCASE(array_add)
{
	int rvals[ELEMCOUNT];
	pcl_array_t *arr = pcl_array_create(10, NULL);

	for(int i = 0; i < ELEMCOUNT; i++)
		rvals[i] = pcl_array_add(arr, (void *) elements[i]);

	bool success = true;

	for(int i = 0; i < ELEMCOUNT; i++)
	{
		if(rvals[i] != i+1 || strcmp((const char *) pcl_array_get(arr, i), elements[i]) != 0)
		{
			success = false;
			break;
		}
	}

	pcl_array_free(arr);
	return success;
}

/**$ Get element at an invalid index */
TESTCASE(array_get_badidx)
{
	pcl_array_t *arr = pcl_array_create(8, NULL);
	void *elem = pcl_array_get(arr, 0);
	bool b = elem == NULL && pcl_errno == PCL_EINDEX;
	pcl_array_free(arr);
	return b;
}

/**$ Get element at a valid index */
TESTCASE(array_get)
{
	pcl_array_t *arr = pcl_array_create(0, NULL);
	int r = pcl_array_add(arr, "value");
	bool b = r == 1 && strcmp((const char *) pcl_array_get(arr, 0), "value") == 0;
	pcl_array_free(arr);
	return b;
}

/**$ Get NULL element at a valid index */
TESTCASE(array_get_null)
{
	pcl_array_t *arr = pcl_array_create(0, NULL);
	int r = pcl_array_add(arr, NULL);
	bool b = r == 1 && pcl_array_get(arr, 0) == NULL && pcl_errno == PCL_EOKAY;
	pcl_array_free(arr);
	return b;
}

/**$ Set element within an array */
TESTCASE(array_set)
{
	pcl_array_t *arr = pcl_array_create(4, NULL);
	int r1 = pcl_array_add(arr, "value1");
	int r2 = pcl_array_add(arr, "value2");
	int r3 = pcl_array_set(arr, "value3", 0);
	bool b = r1 == 1 && r2 == 2 && r3 == 0 &&
		strcmp((const char *) pcl_array_get(arr, 0), "value3") == 0;
	pcl_array_free(arr);
	return b;
}

/**$ Set element beyond current count */
TESTCASE(array_set_beyond_count)
{
	pcl_array_t *arr = pcl_array_create(2, NULL);
	int r = pcl_array_set(arr, "value", 1);
	bool b = r == 0 && pcl_array_get(arr, 0) == NULL && pcl_errno == PCL_EOKAY &&
		arr->count == 2 && strcmp((const char *) pcl_array_get(arr, 1), "value") == 0;
	pcl_array_free(arr);
	return b;
}

/**$ Remove element from array */
TESTCASE(array_remove)
{
	pcl_array_t *arr = pcl_array_create(2, NULL);
	int r = pcl_array_add(arr, "value");
	int r2 = pcl_array_remove(arr, 0);
	bool b = r == 1 && r2 == 0 && arr->count == 0;
	pcl_array_free(arr);
	return b;
}
