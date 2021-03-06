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

#ifndef LIBPCL__JSON_H
#define LIBPCL__JSON_H

#include <pcl/json.h>
#include <pcl/error.h>
#include <pcl/buf.h>

#define JSON_THROW(message, ...) \
	return R_SETERRMSG(NULL, PCL_ESYNTAX, message, __VA_ARGS__)

#define PRINT_TABS(_enc) \
	for(int __i = 0; __i < (enc)->tabs; __i++) \
		pcl_buf_putchar((enc)->b, '\t')

#ifdef __cplusplus
extern "C" {
#endif

typedef struct
{
	const char *next;
	const char *end;
	const char *ctx;
	int line;
} ipcl_json_state_t;

typedef struct
{
	int tabs;
	bool format;
	pcl_buf_t *b;
} ipcl_json_encode_t;

typedef enum
{
	PclPathRoot,
	PclPathMember,
	PclPathWildcardMember,
	PclPathRecursiveDescent,
	PclPathElement,
	PclPathWildcardElement,
	PclPathElementList,
	PclPathElementSlice
} path_type_t;

struct tag_pcl_json_path
{
	pcl_json_path_t *next;

	path_type_t type;

	union
	{
		char *member; // ChildMember
		int index; // ChildElement
		pcl_vector_t *idx_list; // ChildElementList
		struct {
			int start;
			int end;
			int step;
		} idx_slice; // ChildElementSlice
	};
};


PCL_PRIVATE pcl_json_t *ipcl_json_parse_value(ipcl_json_state_t *s);
PCL_PRIVATE char *ipcl_json_parse_string(ipcl_json_state_t *s);
PCL_PRIVATE pcl_json_t *ipcl_json_parse_array(ipcl_json_state_t *s);
PCL_PRIVATE pcl_json_t *ipcl_json_parse_object(ipcl_json_state_t *s);
PCL_PRIVATE pcl_json_t *ipcl_json_parse_number(ipcl_json_state_t *s);
PCL_PRIVATE ipcl_json_state_t *ipcl_json_skipws(ipcl_json_state_t *s);

PCL_PRIVATE pcl_buf_t *ipcl_json_encode_value(ipcl_json_encode_t *enc, pcl_json_t *value);
PCL_PRIVATE pcl_buf_t *ipcl_json_encode_string(ipcl_json_encode_t *enc, const char *string);
PCL_PRIVATE pcl_buf_t *ipcl_json_encode_array(ipcl_json_encode_t *enc, pcl_array_t *array);
PCL_PRIVATE pcl_buf_t *ipcl_json_encode_object(ipcl_json_encode_t *enc, pcl_htable_t *obj);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL__JSON_H
