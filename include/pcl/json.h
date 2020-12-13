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

#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_json_value
{
	/** JSON value type: 0=null, 'n'=number, 'b'=bool, 's'=string, 'a'=array, 'o'=object */
	char type;

	union
	{
		bool boolean;
		double number;
		const char *string;
		pcl_vector_t *array;
		pcl_htable_t *object;
	};
};

/** Parse a JSON string. This parses a JSON string that represents a JSON value. A JSON value
 * can be an object \c {}, array \c [], string \c "", number \c -102.84, boolean \c true or
 * \c false or a null value \c null. Parsing stops when the parsing of the JSON value at \a json
 * is complete, regardless of the \a len argument. However, if a complete JSON object is not found
 * between \a json \c + \a len, an error occurs. The \a json string can contain bogus data
 * after the value being parsed, since the parser stops at the end of the value. For example:
 * @code
 * {
 *   "name": "Henry Ford"
 * }#invalid json here
 * @endcode
 * The above is completely valid, since a valid JSON object exists and ends before any invalid
 * JSON text is encountered. When the \a end argument is supplied, it would be pointing at
 * the '#' in the above example. This is very useful if a string contains multiple JSON values.
 * pcl_json_parse can be called a multiple times passing the value of \a end as \a json.
 *
 * @param json pointer to a json string. This must be NUL terminated if \a len is 0.
 * @param len number of bytes within \a json argument. If 0, \a json must be NUL terminated.
 * @param pointer to the first character not parsed. This can be \c NULL.
 * @return pointer to a json value or \c NULL on error
 */
PCL_EXPORT pcl_json_value_t *pcl_json_parse(const char *json, size_t len, const char **end);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_JSON_H
