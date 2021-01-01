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
#include <pcl/limits.h>

/** Skip UTF-8 validation. Only set this flag if you know for sure a string is valid UTF-8.
 * Otherwise, you can end up with an invalid JSON document that will be rejected by parsers
 * outside of PCL.
 * @see pcl_json_str, pcl_json_strlen
 */
#define PCL_JSON_SKIPUTF8CHK 0x01

/** Shallow copy string. By default, ::pcl_json_str makes a copy of the provided string.
 * Setting this flag avoids the copy and the PCL json module will manage the pointer.
 * @see pcl_json_str
 */
#define PCL_JSON_SHALLOW 0x02

/** Empty strings will be encoded as a JSON null.
 * @see pcl_json_str, pcl_json_strlen
 */
#define PCL_JSON_EMPTYASNULL 0x04

/** A \c NULL string will not raise an error, but rather encode value as a JSON null.
 * @see pcl_json_str, pcl_json_strlen
 */
#define PCL_JSON_ALLOWNULL 0x08

/** When adding json objects to collections, if the add operation fails this flags will free
 * the value being added.
 * @code
 * pcl_json_t *val = pcl_json_int(10);
 *
 * // NULL key will force an error, this will free 'val'
 * pcl_json_objput(obj, NULL, val, PCL_JSON_FREEVALONERR);
 *
 * // identical to
 * if(pcl_json_objput(obj, NULL, val, 0) < 0)
 *   pcl_json_free(val);
 *
 * @endcode
 * @see pcl_json_objput, pcl_json_arradd
 */
#define PCL_JSON_FREEVALONERR 0x10

/** Invalid JSON integer value used as return value.
 * @see pcl_json_objgetint, pcl_json_array_getinteger
 */
#define PCL_JSON_INVINT LONG_LONG_MIN

/** Invalid JSON real value use as return value.
 * @see pcl_json_objgetreal, pcl_json_array_getreal
 */
#define PCL_JSON_INVREAL DBL_MIN

#ifdef __cplusplus
extern "C" {
#endif

#define pcl_json_isnull(_j)    ((_j) ? (_j)->type == 0 : false)
#define pcl_json_isbool(_j) ((_j) ? (_j)->type == 'b' : false)
#define pcl_json_istrue(_j)    ((_j) ? pcl_json_isbool(_j) && (_j)->boolean : false)
#define pcl_json_isfalse(_j)   ((_j) ? pcl_json_isbool(_j) && !(_j)->boolean : false)
#define pcl_json_isreal(_j)    ((_j) ? (_j)->type == 'r' : false)
#define pcl_json_isint(_j) ((_j) ? (_j)->type == 'i' : false)
#define pcl_json_isnum(_j)  (pcl_json_isreal(_j) || pcl_json_isint(_j))
#define pcl_json_isstr(_j)  ((_j) ? (_j)->type == 's' : false)
#define pcl_json_isarr(_j)   ((_j) ? (_j)->type == 'a' : false)
#define pcl_json_isobj(_j)  ((_j) ? (_j)->type == 'o' : false)

struct tag_pcl_json
{
	/** JSON type: 0=null, 'r'=real, 'i'=integer, 'b'=bool, 's'=string, 'a'=array, 'o'=object
	 * Both real and integer are actually a JSON number but are separated for simplicity.
	 */
	char type;

	int nrefs;

	union
	{
		bool boolean;
		double real;
		long long integer;
		/** A NUL-terminated string */
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

PCL_EXPORT pcl_json_path_t *pcl_json_compile(const char *path);

PCL_EXPORT void pcl_json_freepath(pcl_json_path_t *path);

/** Match a JSON value against a compiled JSON Path.
 * @param j pointer to a json value
 * @param path a compiled JSON Path
 * @return a pointer to an array of pcl_json_t values or \c NULL on error. Each json value's
 * reference count has been increased by one. This array can be empty. Free with ::pcl_array_free.
 */
PCL_EXPORT pcl_array_t *pcl_json_match(pcl_json_t *j, const pcl_json_path_t *path);

PCL_EXPORT void pcl_json_free(pcl_json_t *j);

/** Create an object.
 * It is safe to use the @ref htable "hash table module" for managing a JSON object. Internally,
 * a json object is backed by a ::pcl_htable_t. For a given \c pcl_json_t, ensure the \a type
 * member is \c 'o' and then use \c json->object with the htable functions. However, do not
 * use ::pcl_htable_free.
 * @return pointer to a json object of type object
 */
PCL_EXPORT pcl_json_t *pcl_json_obj(void);
PCL_EXPORT int pcl_json_objput(pcl_json_t *obj, char *key, pcl_json_t *value, uint32_t flags);
PCL_EXPORT pcl_json_t *pcl_json_objget(pcl_json_t *obj, const char *key);
PCL_EXPORT const char *pcl_json_objgetstr(pcl_json_t *obj, const char *key);
PCL_EXPORT long long pcl_json_objgetint(pcl_json_t *obj, const char *key);
PCL_EXPORT double pcl_json_objgetreal(pcl_json_t *obj, const char *key);
PCL_EXPORT pcl_htable_t *pcl_json_objgetobj(pcl_json_t *obj, const char *key);
PCL_EXPORT pcl_array_t *pcl_json_objgetarr(pcl_json_t *obj, const char *key);
PCL_EXPORT bool pcl_json_objisnull(pcl_json_t *obj, const char *key);
PCL_EXPORT bool pcl_json_objisbool(pcl_json_t *obj, const char *key);
PCL_EXPORT int pcl_json_objremove(pcl_json_t *obj, const char *key);

/** Create an array object.
 * It is safe to use the @ref array "array module" for managing a JSON array. Internally, a json
 * array is backed by a ::pcl_array_t. For a given \c pcl_json_t, ensure the \a type member is
 * \c 'a' and then use \c json->array with the array functions. However, do not use ::pcl_array_free.
 * @return pointer to a json object of type array
 */
PCL_EXPORT pcl_json_t *pcl_json_arr(void);

/** Add an element to an array.
 * @param arr
 * @param elem
 * @return new element count on success and -1 on error
 */
PCL_EXPORT int pcl_json_arradd(pcl_json_t *arr, pcl_json_t *elem, uint32_t flags);

/**
 *
 * @param arr
 * @param index
 * @return
 */
PCL_EXPORT pcl_json_t *pcl_json_arrget(pcl_json_t *arr, int index);

/**
 *
 * @param arr
 * @param index
 * @return new count or -1 on error
 */
PCL_EXPORT int pcl_json_arrremove(pcl_json_t *arr, int index);

/** Create a string.
 * @param str pointer to a NUL-terminated string
 * @param flags a bitmask of flags. If ::PCL_JSON_SKIPUTF8CHK is set, no UTF-8 validation
 * is performed. If ::PCL_JSON_SHALLOW is set, a direct pointer to \a str is stored: ie. no copy.
 * If ::PCL_JSON_SHALLOW is not set, \a str is copied. Note that if ::PCL_JSON_EMPTYASNULL and
 * ::PCL_JSON_SHALLOW are both set and \a str is empty, caller is still repsonsible to free the
 * empty string. This is because the json module isn't managing it. For this reason, one should
 * avoid setting both of these flags. If ::PCL_JSON_EMPTYASNULL, then an empty \a str
 * will cause a json null object to be returned. If ::PCL_JSON_ALLOWNULL is set, if \a str
 * is \c NULL, a json null object will be returned.
 * @return pointer to a json object or \c NULL on error. Note that a \c NULL \a str is an error
 * unless ::PCL_JSON_ALLOWNULL is set.
 * @see pcl_json_strlen
 */
PCL_EXPORT pcl_json_t *pcl_json_str(char *str, uint32_t flags);

/** Create a string.
 * @note this function always copies \a len characters of the given \a str
 * @param str pointer to string, which does not need to be NUL-terminated
 * @param len number of characters to copy from \a str
 * @param flags bitmask of flags. ::PCL_JSON_SHALLOW is not supported. If ::PCL_JSON_SKIPUTF8CHK
 * is set, no UTF-8 validation is performed. If ::PCL_JSON_EMPTYASNULL, then a \a len of zero
 * will cause a json null object to be returned. If ::PCL_JSON_ALLOWNULL is set, if \a str
 * is \c NULL, a json null object will be returned.
 * @return pointer to a json object or \c NULL on error. Note that a \c NULL \a str is an error
 * unless ::PCL_JSON_ALLOWNULL is set.
 * @see pcl_json_str
 */
PCL_EXPORT pcl_json_t *pcl_json_strlen(const char *str, size_t len, uint32_t flags);

/** Create a boolean.
 * @param value
 * @return
 */
PCL_EXPORT pcl_json_t *pcl_json_bool(bool value);
PCL_EXPORT pcl_json_t *pcl_json_true(void);
PCL_EXPORT pcl_json_t *pcl_json_false(void);
PCL_EXPORT pcl_json_t *pcl_json_null(void);

PCL_EXPORT pcl_json_t *pcl_json_real(double real);
PCL_EXPORT pcl_json_t *pcl_json_int(long long integer);

PCL_INLINE pcl_json_t *
pcl_json_ref(pcl_json_t *j, int amt)
{
	if(j)
		j->nrefs += amt;
	return j;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_JSON_H
