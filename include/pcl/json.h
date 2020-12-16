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

#ifndef LIBPCL_JSON_H
#define LIBPCL_JSON_H

/** @defgroup json JSON Parser
 * A JSON parser.
 * @{
 */
#include <pcl/types.h>

/** Skip UTF-8 validation. Only set this flag if you know for sure a string is valid UTF-8.
 * Otherwise, you can end up with an invalid JSON document that will be regected by parsers
 * outside of PCL.
 * @see pcl_json_string
 */
#define PCL_JSON_SKIPUTF8CHK 0x01

/** Shallow copy string. By default, ::pcl_json_string makes a copy of the provided string.
 * Setting this flag avoids the copy and the PCL json module will manage the pointer.
 * @see pcl_json_string
 */
#define PCL_JSON_SHALLOW 0x02


#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_json
{
	/** JSON type: 0=null, 'r'=real, 'i'=integer, 'b'=bool, 's'=string, 'a'=array, 'o'=object
	 * Both real and integer are actually a JSON number but are separated for simplicity.
	 */
	char type;

	union
	{
		bool boolean;
		double real;
		long long integer;
		char *string;
		pcl_array_t *array;
		pcl_htable_t *object;
	};
};

/** Decode a JSON string. This parses a JSON string that represents a single JSON value. A JSON
 * value can be an object \c {}, array \c [], string \c "", number \c -102.84, boolean \c true or
 * \c false or a null \c null. Parsing stops when the parsing of the JSON value at \a json
 * is complete, regardless of the \a len argument. However, if a complete JSON value is not found
 * before reaching \a json \c + \a len, an error occurs. The \a json string can contain bogus data
 * after the value being parsed, since the parser stops at the end of the value. For example:
 * @code
 * {
 *   "name": "Henry Ford"
 * }#invalid json here
 * @endcode
 * The above is completely valid, since a valid JSON object exists and ends before any invalid
 * JSON text is encountered. When the \a end argument is supplied, it would be pointing at
 * the '#' in the above example. This is very useful if a string contains multiple JSON values.
 * pcl_json_decode can be called a multiple times passing the value of \a end as \a json,
 * allowing one to iterate through a series of json values.
 *
 * @param json pointer to a json string. This must be NUL terminated if \a len is 0.
 * @param len number of bytes within \a json argument. If 0, \a json must be NUL terminated.
 * @param end pointer to the first character not parsed. This can be \c NULL.
 * @return pointer to a json value or \c NULL on error
 */
PCL_EXPORT pcl_json_t *pcl_json_decode(const char *json, size_t len, const char **end);

PCL_EXPORT char *pcl_json_encode(pcl_json_t *value, bool format);

PCL_EXPORT void pcl_json_free(pcl_json_t *j);

/** Create an object.
 * It is safe to use the @ref htable "hash table module" for managing a JSON object. Internally,
 * a json object is backed by a ::pcl_htable_t. For a given \c pcl_json_t, ensure the \a type
 * member is \c 'o' and then use \c json->object with the htable functions. However, do not
 * use ::pcl_htable_free.
 * @return pointer to a json object of type object
 */
PCL_EXPORT pcl_json_t *pcl_json_object(void);
PCL_EXPORT int pcl_json_object_put(pcl_json_t *obj, char *key, pcl_json_t *value);
PCL_EXPORT pcl_json_t *pcl_json_object_get(pcl_json_t *obj, const char *key);

/** Create an array object.
 * It is safe to use the @ref array "array module" for managing a JSON array. Internally, a json
 * array is backed by a ::pcl_array_t. For a given \c pcl_json_t, ensure the \a type member is
 * \c 'a' and then use \c json->array with the array functions. However, do not use ::pcl_array_free.
 * @return pointer to a json object of type array
 */
PCL_EXPORT pcl_json_t *pcl_json_array(void);

/** Add an element to an array.
 * @param arr
 * @param elem
 * @return new element count on success and -1 on error
 */
PCL_EXPORT int pcl_json_array_add(pcl_json_t *arr, pcl_json_t *elem);

/**
 *
 * @param arr
 * @param index
 * @return
 */
PCL_EXPORT pcl_json_t *pcl_json_array_get(pcl_json_t *arr, int index);

/**
 *
 * @param str pointer to a string
 * @param len number of bytes. If this is zero, \a str MUST be NUL-terminated
 * @param flags a bitmask of flags. If PCL_JSON_SHALLOW is set, \a str will not be copied. In
 * this case, do not directly interact with or free \a str after this call! Only interact
 * through pcl_json_t. If PCL_JSON_SKIPUTF8CHK  is set, this will skip UTF-8 validation.
 * @return
 */
PCL_EXPORT pcl_json_t *pcl_json_string(char *str, size_t len, uint32_t flags);

/** Create a boolean.
 * @param value
 * @return
 */
PCL_EXPORT pcl_json_t *pcl_json_boolean(bool value);
PCL_EXPORT pcl_json_t *pcl_json_true(void);
PCL_EXPORT pcl_json_t *pcl_json_false(void);


PCL_EXPORT pcl_json_t *pcl_json_null(void);

PCL_EXPORT pcl_json_t *pcl_json_real(double real);
PCL_EXPORT pcl_json_t *pcl_json_integer(long long integer);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_JSON_H
