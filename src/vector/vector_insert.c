/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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

#include <pcl/vector.h>
#include <pcl/stdlib.h>
#include <string.h>

/* Insert 'elem' at the 'pos'. returns new elem on success and NULL on error.
 * If elem is NULL, a new zero'd elem is inserted and returned.
 */
void *
pcl_vector_insert(pcl_vector_t *v, const void *elem, int pos)
{
	if(pos > v->count)
		return NULL;

	if(v->count == v->capacity)
	{
		v->capacity = (v->capacity * 3) / 2;
		v->elems = (char *) pcl_realloc(v->elems, v->capacity * v->size);
	}

	/* memmove only required when NOT appending: ie. pos '<' count */
	if(pos < v->count)
		memmove(pcl_vector_get(v, pos + 1), pcl_vector_get(v, pos), (v->count - pos) * v->size);

	if(!elem)
		memset(pcl_vector_get(v, pos), 0, v->size);
	else
		memcpy(pcl_vector_get(v, pos), elem, v->size);

	v->count++;
	return pcl_vector_get(v, pos);
}
