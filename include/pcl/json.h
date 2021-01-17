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

#ifndef LIBPCL_JSON_H
#define LIBPCL_JSON_H

/** @defgroup json JSON Decoder and Encoder
 * A JSON encoder and decoder.
 * @{
 */
#include <pcl/types.h>
#include <pcl/limits.h>

/** @defgroup jsonflags Control Flags
 * A set of flags to control the handling of errors and strings.
 * @{
 */

/** Skip UTF-8 validation. Only set this flag if you know for sure a string is valid UTF-8.
 * Otherwise, you can end up with an invalid JSON document that will be rejected by parsers
 * outside of PCL.
 * @see pcl_json_str, pcl_json_strn
 */
#define PCL_JSON_SKIPUTF8CHK 0x01

/** Shallow copy string. By default, all strings being added to a json object are copied. This
 * includes object keys, object string values, array string values and string values. With this
 * flags set, no copy is performed. Any string used with this flag, must point to the beginning
 * of an allocated block; since it will be passed to \c free.
 * @see pcl_json_str, pcl_json_strn, pcl_json_objput, pcl_json_objputstr, pcl_json_arraddstr
 */
#define PCL_JSON_SHALLOW 0x02

/** Treat empty strings as a JSON null.
 * @see pcl_json_str, pcl_json_strn, pcl_json_objputstr, pcl_json_arraddstr
 */
#define PCL_JSON_EMPTYASNULL 0x04

/** Treat \c NULL string pointers as a JSON null. Without this flag, a \c NULL string wlll
 * instead be treated as an error.
 * @see pcl_json_str, pcl_json_strn
 */
#define PCL_JSON_ALLOWNULL 0x08

/** Free a json value being added when the operation fails.
 * When adding json values to objects or arrays, if the add operation fails this flag causes
 * the value being added to be freed.
 * @code
 * pcl_json_t *val = pcl_json_int(10);
 *
 * // this will free 'val' if it fails
 * pcl_json_objput(obj, key, val, PCL_JSON_FREEVALONERR);
 *
 * // identical to
 * if(pcl_json_objput(obj, key, val, 0) < 0)
 *   pcl_json_free(val);
 *
 * @endcode
 * @see pcl_json_objput, pcl_json_arradd
 */
#define PCL_JSON_FREEVALONERR 0x10

/** @} */

/** Invalid JSON integer value used as return value.
 * @see pcl_json_objgetint, pcl_json_arrgetint
 */
#ifdef PCL_WINDOWS
#	define PCL_JSON_INVINT LLONG_MIN
#else
#	define PCL_JSON_INVINT LONG_LONG_MIN
#endif

/** Invalid JSON real value use as return value.
 * @see pcl_json_objgetreal, pcl_json_arrgetreal
 */
#define PCL_JSON_INVREAL DBL_MIN

/** Indicates if a json object is a null value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isnull(_j)    ((_j) ? (_j)->type == 0 : false)

/** Indicates if a json object is a boolean value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isbool(_j)    ((_j) ? (_j)->type == 'b' : false)

/** Indicates if a json object is a boolean true value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_istrue(_j)    ((_j) ? pcl_json_isbool(_j) && (_j)->boolean : false)

/** Indicates if a json object is a boolean false value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isfalse(_j)   ((_j) ? pcl_json_isbool(_j) && !(_j)->boolean : false)

/** Indicates if a json object is a real value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isreal(_j)    ((_j) ? (_j)->type == 'r' : false)

/** Indicates if a json object is an integer value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isint(_j)     ((_j) ? (_j)->type == 'i' : false)

/** Indicates if a json object is a number (integer or real) value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isnum(_j)     (pcl_json_isreal(_j) || pcl_json_isint(_j))

/** Indicates if a json object is a string value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isstr(_j)     ((_j) ? (_j)->type == 's' : false)

/** Indicates if a json object is an array value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isarr(_j)     ((_j) ? (_j)->type == 'a' : false)

/** Indicates if a json object is an object value.
 * @param _j pointer to a json object
 * @return boolean
 */
#define pcl_json_isobj(_j)     ((_j) ? (_j)->type == 'o' : false)

#ifdef __cplusplus
extern "C" {
#endif

/** A JSON value object. This represents any valid JSON value: boolean, number (integer or real),
 * string, null, array or object.
 */
struct tag_pcl_json
{
	/** JSON value type. This can be: \c 0 for null, \c 'r' for real, \c 'i' for integer,
	 * \c 'b' for bool, \c 's' for string, \c 'a' for array, \c 'o' for object. Both real and
	 * integer are actually a JSON number but are separated for simplicity. There are macros
	 * for detecting value type that all begin with \c pcl_json_is.
	 */
	char type;

	/** A JSON object's reference count.
	 * @see pcl_json_ref
	 */
	int nrefs;

	union
	{
		/** JSON boolean value when \a type is \c 'b'. */
		bool boolean;

		/** JSON real (number) value when \a type is \c 'r'. THis cannot be NaN, Infinity or
		 * ::PCL_JSON_INVREAL.
		 */
		double real;

		/** JSON integer (number) value when \a type is \c 'i'. This cannot be ::PCL_JSON_INVINT. */
		long long integer;

		/** JSON string value when \a type is \c 's'. This is always NUL-terminated. */
		char *string;

		/** JSON array value when \a type is \c 'a'. */
		pcl_array_t *array;

		/** JSON object value when \a type is \c 'o'. */
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
 * pcl_json_decode can be called multiple times by passing the value of \a end as \a json,
 * allowing one to iterate through a series of json values.
 *
 * @param json pointer to a json string. This must be NUL terminated if \a len is 0.
 * @param len number of bytes within \a json argument. If 0, \a json must be NUL terminated.
 * @param end pointer to the first character not parsed. This can be \c NULL.
 * @return pointer to a json value or \c NULL on error
 */
PCL_PUBLIC pcl_json_t *pcl_json_decode(const char *json, size_t len, const char **end);

/** Encode a json object as a string.
 * @param value pointer to a json object
 * @param format if true, spaces and tabs will be added to the output.
 * @return poin ter to an allocated json string
 */
PCL_PUBLIC char *pcl_json_encode(pcl_json_t *value, bool format);

/** Compile a json path.
 * @param path A JSONPath string. This doesn't support filters.
 * @return pointer to a compiled json path object which must be freed with ::pcl_json_freepath.
 */
PCL_PUBLIC pcl_json_path_t *pcl_json_compile(const char *path);

/** Release all resources used by a json path object.
 * @param path pointer to a path object.
 */
PCL_PUBLIC void pcl_json_freepath(pcl_json_path_t *path);

/** Match a JSON value against a compiled JSON Path.
 * @param j pointer to a json value
 * @param path a compiled JSON Path
 * @return a pointer to an array of pcl_json_t values or \c NULL on error. Each json value's
 * reference count has been increased by one. This array can be empty. Free with ::pcl_array_free.
 */
PCL_PUBLIC pcl_array_t *pcl_json_match(pcl_json_t *j, const pcl_json_path_t *path);

/** Query a json object with a path.
 * Internally, this performs a ::pcl_json_compile followed by ::pcl_json_match. Although this
 * is convenient, if a compiled path is to be used across many documents, it is better to
 * compile and then use the match function.
 * @param j pointer to a json object
 * @param path json path
 * @return a pointer to an array of pcl_json_t values or \c NULL on error. Each json value's
 * reference count has been increased by one. This array can be empty. Free with ::pcl_array_free.
 */
PCL_PUBLIC pcl_array_t *pcl_json_query(pcl_json_t *j, const char *path);

/** Release all resources used by a json value. When the given json object's reference count is
 * one, it will be freed. If greater than one, its reference count will be decremeted but it
 * will not be freed.
 * @param j pointer to a json object
 * @see pcl_json_ref
 */
PCL_PUBLIC void pcl_json_free(pcl_json_t *j);

/** Get count of array elements or object keys.
 * @param j json value which must be an object ore array
 * @return number of items or -1 on error
 */
PCL_PUBLIC int pcl_json_count(const pcl_json_t *j);

/** @defgroup jsonobj Object Management
 * A set of functions for managing JSON objects of type object.
 * @{
 */

/** Create an object.
 * It is safe to use the @ref htable "hash table module" for managing a JSON object. Internally,
 * a json object is backed by a ::pcl_htable_t. For a given \c pcl_json_t, ensure the \a type
 * member is \c 'o' and then use \c json->object with the htable functions. However, do not
 * use ::pcl_htable_free.
 * @return pointer to a json object of type object
 */
PCL_PUBLIC pcl_json_t *pcl_json_obj(void);

/** Put a json value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param value pointer to a json value
 * @param flags a bitmask of @ref jsonflags
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objput(pcl_json_t *obj, char *key, pcl_json_t *value, uint32_t flags);

/** Put a string value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param value pointer to a string value
 * @param flags a bitmask of @ref jsonflags. String control flags apply to both \a key and \a value
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objputstr(pcl_json_t *obj, char *key, char *value, uint32_t flags);

/** Put an integer value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param value integer value
 * @param flags a bitmask of @ref jsonflags
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objputint(pcl_json_t *obj, char *key, long long value, uint32_t flags);

/** Put a real value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param value real value
 * @param flags a bitmask of @ref jsonflags
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objputreal(pcl_json_t *obj, char *key, double value, uint32_t flags);

/** Put a boolean value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param value boolean value
 * @param flags a bitmask of @ref jsonflags
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objputbool(pcl_json_t *obj, char *key, bool value, uint32_t flags);

/** Put a null value into an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @param flags a bitmask of @ref jsonflags
 * @return 0 for success or -1 on error
 */
PCL_PUBLIC int pcl_json_objputnull(pcl_json_t *obj, char *key, uint32_t flags);

/** Get a json value from an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @return pointer to a json object.
 */
PCL_PUBLIC pcl_json_t *pcl_json_objget(const pcl_json_t *obj, const char *key);

/** Get a string value from an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @return pointer to a string that is the value for \a key. The returned string is managed
 * by \a obj and a copy is required if it is to persist past the life of \a obj. On error,
 * \c NULL is returned.
 */
PCL_PUBLIC const char *pcl_json_objgetstr(const pcl_json_t *obj, const char *key);

/** Get a integer value from an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @return integer value found at \a key. If an error occurs, ::PCL_JSON_INVINT is returned.
 */
PCL_PUBLIC long long pcl_json_objgetint(const pcl_json_t *obj, const char *key);

/** Get a real value from an object.
 * @param obj pointer to a json object of type object
 * @param key pointer to a string key
 * @return double value found at \a key. If an error occurs, ::PCL_JSON_INVREAL is returned.
 */
PCL_PUBLIC double pcl_json_objgetreal(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a string.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a string and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisstr(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is an integer.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is an integer and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisint(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a real.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a real and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisreal(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is an object.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is an object and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisobj(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is an array.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is an array and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisarr(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a null.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a null and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisnull(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a boolean.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a boolean and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisbool(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a boolean true.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a boolean true and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objistrue(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a boolean false.
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a boolean false and false otherwise.
 */
PCL_PUBLIC bool pcl_json_objisfalse(const pcl_json_t *obj, const char *key);

/** Indicates if an object key's value is a number (real or integer).
 * @param arr pointer to a json object of type object
 * @param key pointer to a string
 * @return true if key's value is a number and false otherwise.
 */
#define pcl_json_objisnum(obj, key) (pcl_json_objisint(obj, key) || pcl_json_objisreal(obj, key))

/** Remove a value from a json object.
 * @param obj pointer to a json object of type object.
 * @param key pointer to a string
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_objremove(pcl_json_t *obj, const char *key);

/** @} */

/** @defgroup jsonarr Array Management
 * A set of functions for managing JSON objects of type array.
 * @{
 */

/** Create an array object.
 * It is safe to use the @ref array "array module" for managing a JSON array. Internally, a json
 * array is backed by a ::pcl_array_t. For a given \c pcl_json_t, ensure the \a type member is
 * \c 'a' and then use \c json->array with the array functions. However, do not use ::pcl_array_free.
 * @return pointer to a json object of type array
 */
PCL_PUBLIC pcl_json_t *pcl_json_arr(void);

/** Add an element to an array.
 * @param arr pointer to a json array object
 * @param elem pointer to a json object of any type
 * @param flags This only suports ::PCL_JSON_FREEVALONERR.
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arradd(pcl_json_t *arr, pcl_json_t *elem, uint32_t flags);

/** Add a string value to an array.
 * @param arr pointer to a json array object
 * @param value pointer to a string.
 * @param flags For \a value, all flags supported by ::pcl_json_str are supported here. This also
 * supprts ::PCL_JSON_FREEVALONERR.
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arraddstr(pcl_json_t *arr, char *value, uint32_t flags);

/** Add an integer value to an array.
 * @param arr pointer to a json array object
 * @param value integer value. This cannot be ::PCL_JSON_INVINT.
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arraddint(pcl_json_t *arr, long long value);

/** Add a real value to an array.
 * @param arr pointer to a json array object
 * @param value real value. This cannot be ::PCL_JSON_INVREAL, NaN or Infinity.
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arraddreal(pcl_json_t *arr, double value);

/** Add a boolean value to an array.
 * @param arr pointer to a json array object
 * @param value boolean value
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arraddbool(pcl_json_t *arr, bool value);

/** Add a null value to an array.
 * @param arr pointer to a json array object
 * @return 0 on success and -1 on error
 */
PCL_PUBLIC int pcl_json_arraddnull(pcl_json_t *arr);

/** Get a json object from an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return pointer to a json object that is only valid while \a arr is valid.
 * @see pcl_json_ref
 */
PCL_PUBLIC pcl_json_t *pcl_json_arrget(const pcl_json_t *arr, int index);

/** Get a string from an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return pointer to a string that is the value at \a index. The returned string is managed
 * by \a arr and a copy is required if it is to persist past the life of \a arr. On error,
 * \c NULL is returned.
 */
PCL_PUBLIC const char *pcl_json_arrgetstr(const pcl_json_t *arr, int index);

/** Get an integer (number) from an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return integer value found at \a index. If an error occurs, ::PCL_JSON_INVINT is returned.
 */
PCL_PUBLIC long long pcl_json_arrgetint(const pcl_json_t *arr, int index);

/** Get a real (number) from an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return double value found at \a index. If an error occurs, ::PCL_JSON_INVREAL is returned.
 */
PCL_PUBLIC double pcl_json_arrgetreal(const pcl_json_t *arr, int index);

/** Indicates if an array element is a string.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is a string and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisstr(const pcl_json_t *arr, int index);

/** Indicates if an array element is an integer.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is an integer and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisint(const pcl_json_t *arr, int index);

/** Indicates if an array element is a real.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is a real and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisreal(const pcl_json_t *arr, int index);

/** Indicates if an array element is an object.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is an object and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisobj(const pcl_json_t *arr, int index);

/** Indicates if an array element is an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is an array and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisarr(const pcl_json_t *arr, int index);

/** Indicates if an array element is a null.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is a null and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisnull(const pcl_json_t *arr, int index);

/** Indicates if an array element is a boolean.
 * @param arr pointer to a json array object
 * @param index element index
 * @return true if element is a boolean and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisbool(const pcl_json_t *arr, int index);

/** Indicates if an array element is a boolean true.
 * @param arr pointer to a json array object
 * @param idx element index
 * @return true if element is a boolean true and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arristrue(const pcl_json_t *arr, int index);

/** Indicates if an array element is a boolean false.
 * @param arr pointer to a json array object
 * @param idx element index
 * @return true if element is a boolean false and false otherwise.
 */
PCL_PUBLIC bool pcl_json_arrisfalse(const pcl_json_t *arr, int index);

/** Indicates if an array element is a number.
 * @param arr pointer to a json array object
 * @param idx element index
 * @return true if element is an integer or real and false otherwise.
 */
#define pcl_json_arrisnum(arr, idx) (pcl_json_arrisint(arr, idx) || pcl_json_arrisreal(arr, idx))

/** Remove an element from an array.
 * @param arr pointer to a json array object
 * @param index element index
 * @return new count or -1 on error
 */
PCL_PUBLIC int pcl_json_arrremove(pcl_json_t *arr, int index);

/** @} */

/** Create a string object.
 * @param str pointer to a NUL-terminated string
 * @param flags bitmask of @ref jsonflags. ::PCL_JSON_FREEVALONERR is ignored.
 * @return pointer to a json object or \c NULL on error. Note that a \c NULL \a str is an error
 * unless ::PCL_JSON_ALLOWNULL is set.
 * @see pcl_json_strn
 */
PCL_PUBLIC pcl_json_t *pcl_json_str(char *str, uint32_t flags);

/** Create a string object.
 * @note this function always copies \a len characters of the given \a str. Shallow copy is not
 * supported.
 * @param str pointer to string, which does not need to be NUL-terminated
 * @param len number of characters to copy from \a str
 * @param flags bitmask of @ref jsonflags. ::PCL_JSON_SHALLOW and ::PCL_JSON_FREEVALONERR are
 * ignored.
 * @return pointer to a json object or \c NULL on error. Note that a \c NULL \a str is an error
 * unless ::PCL_JSON_ALLOWNULL is set.
 * @see pcl_json_str
 */
PCL_PUBLIC pcl_json_t *pcl_json_strn(const char *str, size_t len, uint32_t flags);

/** Create a boolean value.
 * @note this function returns a singleton
 * @param value boolean value, \c true or \c false.
 * @return pointer to a json boolean object
 */
PCL_PUBLIC pcl_json_t *pcl_json_bool(bool value);

/** Create a boolean true value.
 * @note this function returns a singleton
 * @return pointer to a json boolean object
 */
PCL_PUBLIC pcl_json_t *pcl_json_true(void);

/** Create a boolean false value.
 * @note this function returns a singleton
 * @return pointer to a json boolean object
 */
PCL_PUBLIC pcl_json_t *pcl_json_false(void);

/** Create a null value.
 * @note this function returns a singleton
 * @return pointer to a json null object
 */
PCL_PUBLIC pcl_json_t *pcl_json_null(void);

/** Create a json real (number) object.
 * @param real value. This value cannot be NaN, Infinity or ::PCL_JSON_INVREAL
 * @return pointer to a json real object
 */
PCL_PUBLIC pcl_json_t *pcl_json_real(double real);

/** Create a json integer (number) object.
 * @param integer value. This value cannot be ::PCL_JSON_INVINT
 * @return pointer to a json integer object
 */
PCL_PUBLIC pcl_json_t *pcl_json_int(long long integer);

/** Increase or decrease a json object's reference count.
 * A json object is not freed by ::pcl_json_free until its reference count is zero.
 * @param j pointer to a json object
 * @param amt amount to increase or decrease (negative) the object's reference count.
 * @return pointer to \a j
 * @see pcl_json_free
 */
PCL_INLINE pcl_json_t *
pcl_json_ref(pcl_json_t *j, int amt)
{
	if(j && j != pcl_json_null() && j != pcl_json_true() && j != pcl_json_false())
		j->nrefs += amt;
	return j;
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_JSON_H
