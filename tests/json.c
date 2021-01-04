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
#include <pcl/json.h>
#include <pcl/alloc.h>
#include <pcl/array.h>
#include <string.h>

/* PCL json object uses the PCL hashtable, which does not store insertion order or provide any
 * kind of sorting mechanism. However, the below is predicatble since the same hash func is
 * used for any json parsing. So, the below is the output of a pcl_json_encode. This means
 * any json object created with the correct keys and values, will encode to the same value as
 * below. Double-quote and unicode chars were manually escaped.
 */
#define ENCODED_TEST_FILE "{\"str-ascii-escape\":\"Unit \\u001f Separator\",\"array-objects\":[{\"str-utf16\":\"CJK UNIFIED IDEOGRAPH 阳 and 好\",\"real\":909374653.6736,\"array\":[0,false,\"string\",[1,2,3],{\"a\":\"b\"}],\"str-utf16-surrogate\":\"MUSICAL SYMBOL G CLEF (1D11E) 𝄞\"},{\"key\":\"string\"},{\"key\":null}],\"integer\":9223372036854775807,\"negative-real-exp\":-19999390000,\"null\":null,\"true\":true,\"negative-real\":-83765523.234874,\"empty-object\":{},\"object\":{\"array\":[12,-1273.273,\"string\",true,null,{},[]],\"math symbols\":\"∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i), ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β)\"},\"array\":[\"אָדוֹם\",\"အပြာ\",\"zelená\",\"黄\",\"purple\"],\"empty-array\":[],\"negative-integer\":-9223372036854775807,\"false\":false,\"real-exp\":19999390000,\"real\":83765523.234874}"

static char *loadjson(int *lenp)
{
	FILE *fp = fopen("test-data.json", "r");

	ASSERT_NOTNULL(fp, "failed to open test-data.json");

	size_t len = 0, r;
	static char data[8192];

	while((r = fread(data + len, 1, sizeof(data) - len, fp)) > 0)
		len += r;

	fclose(fp);

	if(lenp)
		*lenp = (int) len;

	return data;
}

/**$ Decode a json string */
TESTCASE(json_decode)
{
	const char *end;
	int len;
	char *data = loadjson(&len);
	pcl_json_t *root = pcl_json_decode(data, (int) len, &end);

	ASSERT_NOTNULL(root, "failed to decode json string");
	ASSERT_TRUE(pcl_json_isobj(root), "root is not an object");
	ASSERT_INTEQ(*end, '#', "wrong value for json_decode 'end' pointer");

	const char *str = pcl_json_objgetstr(root, "str-ascii-escape");
	ASSERT_NOTNULL(str, "str-ascii-escape objgetstr failed");
	ASSERT_STREQ(str, "Unit \x1f Separator", "str-ascii-escape string mismatch");
	ASSERT_TRUE(pcl_json_objisfalse(root, "false"), "boolean false value");
	ASSERT_TRUE(pcl_json_objistrue(root, "true"), "boolean true value");
	ASSERT_TRUE(pcl_json_objisnull(root, "null"), "null value");
	ASSERT_INTEQ(pcl_json_objgetint(root, "integer"), 9223372036854775807LL, "integers do not match");
	ASSERT_INTEQ(pcl_json_objgetint(root, "negative-integer"), -9223372036854775807LL,
		"negative integers do not match");

	ASSERT_DOUBLEEQ(pcl_json_objgetreal(root, "real"), 83765523.234874, "doubles do not match");
	ASSERT_DOUBLEEQ(pcl_json_objgetreal(root, "negative-real"), -83765523.234874,
		"negative doubles do not match");

	ASSERT_DOUBLEEQ(pcl_json_objgetreal(root, "real-exp"), 1999939e4, "double exponents do not match");
	ASSERT_DOUBLEEQ(pcl_json_objgetreal(root, "negative-real-exp"), -1999939e4,
		"negative double exponents do not match");

	pcl_json_t *val = pcl_json_objget(root, "empty-object");
	ASSERT_TRUE(pcl_json_isobj(val), "wrong type for key empty-object");
	ASSERT_INTEQ(pcl_json_count(val), 0, "empty-object is not empty");

	val = pcl_json_objget(root, "empty-array");
	ASSERT_TRUE(pcl_json_isarr(val), "wrong type for key empty-array");
	ASSERT_INTEQ(pcl_json_count(val), 0, "empty-array is not empty");

	val = pcl_json_objget(root, "array");
	ASSERT_TRUE(pcl_json_isarr(val), "wrong type for key array");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 0), "אָדוֹם", "wrong value for array index 0");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 1), "အပြာ", "wrong value for array index 1");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 2), "zelená", "wrong value for array index 2");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 3), "黄", "wrong value for array index 3");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 4), "purple", "wrong value for array index 4");

	val = pcl_json_objget(root, "object");
	const char *symbols = "∮ E⋅da = Q,  n → ∞, ∑ f(i) = ∏ g(i), ∀x∈ℝ: ⌈x⌉ = −⌊−x⌋, α ∧ ¬β = ¬(¬α ∨ β)";
	ASSERT_TRUE(pcl_json_isobj(val), "wrong type for key object");
	ASSERT_STREQ(pcl_json_objgetstr(val, "math symbols"), symbols, "wrong value for key math symbols");

	val = pcl_json_objget(val, "array");
	ASSERT_TRUE(pcl_json_isarr(val), "wrong type for nested key array");
	ASSERT_INTEQ(pcl_json_count(val), 7, "wrong count for nested key array");
	ASSERT_INTEQ(pcl_json_arrgetint(val, 0), 12, "wrong value for nested array index 0");
	ASSERT_DOUBLEEQ(pcl_json_arrgetreal(val, 1), -1273.273, "wrong value for nested array index 1");
	ASSERT_STREQ(pcl_json_arrgetstr(val, 2), "string", "wrong value for nested array index 2");
	ASSERT_TRUE(pcl_json_arristrue(val, 3), "wrong value for nested array index 3");
	ASSERT_TRUE(pcl_json_arrisnull(val, 4), "wrong value for nested array index 4");
	ASSERT_TRUE(pcl_json_arrisobj(val, 5), "wrong value for nested array index 5");
	ASSERT_TRUE(pcl_json_arrisarr(val, 6), "wrong value for nested array index 6");

	val = pcl_json_objget(root, "array-objects");
	ASSERT_TRUE(pcl_json_isarr(val), "wrong type for key array-objects");
	ASSERT_INTEQ(pcl_json_count(val), 3, "wrong count for key array_objects");

	pcl_json_t *obj = pcl_json_arrget(val, 0);
	ASSERT_TRUE(pcl_json_isobj(obj), "wrong type for array-objects index 0");
	ASSERT_STREQ(pcl_json_objgetstr(obj, "str-utf16"), "CJK UNIFIED IDEOGRAPH \u9633 and \u597d",
		"wrong value for str-urf16 key");
	ASSERT_STREQ(pcl_json_objgetstr(obj, "str-utf16-surrogate"),
		"MUSICAL SYMBOL G CLEF (1D11E) \U0001D11E", "wrong value for str-urf16 key");
	ASSERT_DOUBLEEQ(pcl_json_objgetreal(obj, "real"), 909374653.6736, "wrong value for real key");

	pcl_json_t *arr = pcl_json_objget(obj, "array");
	ASSERT_TRUE(pcl_json_isarr(arr), "wrong type for array key");
	ASSERT_INTEQ(pcl_json_count(arr), 5, "wrong count for array");
	ASSERT_INTEQ(pcl_json_arrgetint(arr, 0), 0, "wrong value for array index 0");
	ASSERT_TRUE(pcl_json_arrisfalse(arr, 1), "wrong value for array index 1");
	ASSERT_STREQ(pcl_json_arrgetstr(arr, 2), "string", "wrong value for array index 2");

	pcl_json_t *arr2 = pcl_json_arrget(arr, 3);
	ASSERT_TRUE(pcl_json_isarr(arr2), "wrong type for array index 3");
	ASSERT_INTEQ(pcl_json_count(arr2), 3, "wrong count for array index 3");
	obj = pcl_json_arrget(arr, 4);
	ASSERT_TRUE(pcl_json_isobj(obj), "wrong type for array index 4");
	ASSERT_INTEQ(pcl_json_count(obj), 1, "wrong count array index 4");
	pcl_json_free(root);

	return true;
}

/**$ Encode a json object */
TESTCASE(json_encode)
{
	int len;
	char *data = loadjson(&len);
	pcl_json_t *root = pcl_json_decode(data, (int) len, NULL);

	ASSERT_NOTNULL(root, "failed to decode json string");
	ASSERT_TRUE(pcl_json_isobj(root), "root is not an object");

	char *s = pcl_json_encode(root, false);
	ASSERT_STREQ(s, ENCODED_TEST_FILE, "wrong encoded value");
	pcl_free(s);
	pcl_json_free(root);

	return true;
}

/**$ Use a JSON path to query for json values */
TESTCASE(json_match)
{
	int len;
	char *data = loadjson(&len);
	pcl_json_t *root = pcl_json_decode(data, (int) len, NULL);

	ASSERT_NOTNULL(root, "failed to decode json string");
	ASSERT_TRUE(pcl_json_isobj(root), "root is not an object");

	pcl_json_path_t *path = pcl_json_compile("$['array-objects'][0].real");
	ASSERT_NOTNULL(path, "failed to compile path");

	pcl_array_t *arr = pcl_json_match(root, path);
	ASSERT_NOTNULL(arr, "match failed");
	ASSERT_INTEQ(arr->count, 1, "wrong count for match results");

	pcl_json_t *val = pcl_array_get(arr, 0);
	ASSERT_TRUE(pcl_json_isreal(val), "wrong type for match results");
	ASSERT_INTEQ(val->nrefs, 2, "wrong number of references for match results");
	ASSERT_DOUBLEEQ(val->real, 909374653.6736, "wrong value for matched result");

	pcl_array_free(arr);
	pcl_json_freepath(path);

	path = pcl_json_compile("$..array");
	ASSERT_NOTNULL(path, "failed to compile path");

	arr = pcl_json_match(root, path);
	ASSERT_NOTNULL(arr, "match failed recursive decsent");
	ASSERT_INTEQ(arr->count, 3, "wrong count for match results");

	for(int i = 0; i < arr->count; i++)
	{
		pcl_json_t *elem = pcl_array_get(arr, i);
		ASSERT_TRUE(pcl_json_isarr(elem), "wrong type in array results");
		ASSERT_INTEQ(elem->nrefs, 2, "wrong reference count for matched result");
	}

	pcl_array_free(arr);
	pcl_json_freepath(path);

	path = pcl_json_compile("$['array-objects'][0].array[1,2]");
	ASSERT_NOTNULL(path, "failed to compile path");

	arr = pcl_json_match(root, path);
	ASSERT_NOTNULL(arr, "match failed index set");
	ASSERT_INTEQ(arr->count, 2, "wrong count for match results");

	val = arr->elements[0];
	ASSERT_TRUE(pcl_json_isfalse(val), "wrong type for result 0");
	// true, false and null are always zero
	ASSERT_INTEQ(val->nrefs, 0, "wrong reference count for matched result");
	val = arr->elements[1];
	ASSERT_TRUE(pcl_json_isstr(val), "wrong type for result 1");
	ASSERT_INTEQ(val->nrefs, 2, "wrong reference count for matched result");

	pcl_array_free(arr);
	pcl_json_freepath(path);

	path = pcl_json_compile("$['array-objects'][0].array[-1:]");
	ASSERT_NOTNULL(path, "failed to compile path");

	arr = pcl_json_match(root, path);
	ASSERT_NOTNULL(arr, "match failed slice -1");
	ASSERT_INTEQ(arr->count, 1, "wrong count for match results");

	val = arr->elements[0];
	ASSERT_TRUE(pcl_json_isobj(val), "wrong type for result 0");
	ASSERT_INTEQ(val->nrefs, 2, "wrong reference count for matched result");

	pcl_array_free(arr);
	pcl_json_freepath(path);

	path = pcl_json_compile("$.array[*]");
	ASSERT_NOTNULL(path, "failed to compile path");

	arr = pcl_json_match(root, path);
	ASSERT_NOTNULL(arr, "match failed slice -1");
	ASSERT_INTEQ(arr->count, 5, "wrong count for match results");
	for(int i = 0; i < arr->count; i++)
	{
		val = arr->elements[i];
		ASSERT_TRUE(pcl_json_isstr(val), "wrong type for matched results");
	}

	pcl_array_free(arr);
	pcl_json_freepath(path);
	pcl_json_free(root);

	return true;
}

/**$ Query a json document */
TESTCASE(json_query)
{
	int len;
	char *data = loadjson(&len);
	pcl_json_t *root = pcl_json_decode(data, (int) len, NULL);

	ASSERT_NOTNULL(root, "failed to decode json string");
	ASSERT_TRUE(pcl_json_isobj(root), "root is not an object");

	pcl_array_t *arr = pcl_json_query(root, "$['array-objects'][2].key");
	ASSERT_NOTNULL(arr, "match failed");
	ASSERT_INTEQ(arr->count, 1, "wrong count for match results");

	pcl_json_t *val = arr->elements[0];
	ASSERT_TRUE(pcl_json_isnull(val), "wrong value for match results");

	pcl_array_free(arr);
	pcl_json_free(root);

	return true;
}
