
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

#ifndef LIBPCL_VECTOR_H
#define LIBPCL_VECTOR_H

/* NOTE: When storing pointers, you need to store a double pointer. The most common use case is
 * for a vector of strings.
 *
 * Example:
 * char *elem = pcl_strdup("something");
 * pcl_vector_insert(v, (const void *)&elem, v->count);// or pcl_vector_append(v, &elem)
 *
 * // Make sure to dereference when getting elements like these
 * char *elem = *(char **)pcl_vector_get(v, 0);
 *
 * // To avoid manual dereference everywhere, use getptr versions:
 * char *elem = pcl_vector_getptr(v, 0);
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pcl_vector_cleanup_t)(pcl_vector_t *v, void *elem);

struct tag_pcl_vector
{
	/** number of elements in vector */
	int count;

	/** allocation size, as number of elements */
	int capacity;

	/** size in bytes of an element */
	size_t size;

	/** Element array. This is always (capacity * size) bytes */
	char *elems;

	/** Optional user-defined pointer passed to the cleanup callback. */
	void *cleanup_ptr;

	/** Element cleanup callback. Optionally, one can set cleanup_ptr and access it
	 * within the cleanup() callback: `myobj *o = (myobj *)v->cleanup_ptr;`.
 	 */
	pcl_vector_cleanup_t cleanup;
};

/* no error or bounds checking. For use within a loop constrained by
 * something like "num < vector->count".
 */
#define pcl_vector_get(v, pos) \
	((void *)(((pcl_vector_t *)(v))->elems + ((pos) * ((pcl_vector_t *)(v))->size)))

/* Same as pcl_vector_get except this will dereference a pointer, meaning
 * the element is a double pointer like a string.  This is a
 * convenience macro only, doesn't offer any new behavior.
 */
#define pcl_vector_getptr(v, pos) *((void **)pcl_vector_get(v, pos))

/* Appends an element to the end of vector.  If 'elem' is NULL, a zero'd
 * element is added and a ptr to it is returned. See pcl_vector_insert().
 */
#define pcl_vector_append(v, elem) \
	pcl_vector_insert(v, (const void *)(elem), ((pcl_vector_t *)(v))->count)

PCL_EXPORT pcl_vector_t *pcl_vector_init(pcl_vector_t *v, int capacity,
	size_t elemsize, pcl_vector_cleanup_t cleanup);

/* Creats a new vector with an initial capacity, element size and
 * optional element cleanup callback.
 */
PCL_EXPORT pcl_vector_t *pcl_vector_create(int capacity, size_t elemsize,
	pcl_vector_cleanup_t cleanup);

/* Insert 'elem' at 'pos'. returns new elem on success and NULL on error.
 * If elem is NULL, a zero'd elem is inserted and returned.
 */
PCL_EXPORT void *pcl_vector_insert(pcl_vector_t *v, const void *elem, int pos);

/* same as pcl_vector_append, except this will allocate 's' and then
 * append it. If 's' is NULL, a zero'd element is added. Returns a ptr
 * to the new vector element or NULL on error.
 */
PCL_EXPORT void *pcl_vector_append_str(pcl_vector_t *v, const char *s);

/* Removes the element found at 'pos' from the given vector.  This
 * returns zero on success and -1 on error.
 */
PCL_EXPORT int pcl_vector_remove(pcl_vector_t *v, int pos);

/* (Safe version) Verify the element at the given position.  Returns NULL on
 * error and sets pcl_errno. To avoid bounds checking, see pcl_vector_get and
 * pcl_vector_getptr macros.
 */
PCL_EXPORT void *pcl_vector_get_s(pcl_vector_t *v, int pos);

/* Same as pcl_vector_get_s() except this will dereference double pointer
 * elements, like a vector of strings would contain. Returns elem at pos
 * or NULL on error. To avoid bounds checking, see pcl_vector_get and
 * pcl_vector_getptr macros.
 */
PCL_EXPORT void *pcl_vector_getptr_s(pcl_vector_t *v, int pos);

/* Compact a vector, ensure its capacity is the same as its element count. */
PCL_EXPORT pcl_vector_t *pcl_vector_compact(pcl_vector_t *v);

/* Clear all elements from vector but do not release vector resources.
 * This allows a pcl_vector_t object to be reused.  Keep in mind, if a vector
 * previously had 1,000,000 elements then after this clear the vector
 * will still have a capacity of at least 1,000,000.  In some cases, calling
 * pcl_vector_compact() after this could make sense.
 */
PCL_EXPORT pcl_vector_t *pcl_vector_clear(pcl_vector_t *v);

/* Release all vector resources excluding the pcl_vector_t object. This is
 * useful when pcl_vector_init is used and the caller allocates (or uses stack)
 * for the vector object. The vector object is zero'd and must be re-initialized.
 */
PCL_EXPORT pcl_vector_t *pcl_vector_uninit(pcl_vector_t *v);

PCL_EXPORT void pcl_vector_free(pcl_vector_t *v);

/* Built-in cleanup for double pointer elements, like strings (char**). */
PCL_EXPORT void pcl_vector_cleanup_dblptr(pcl_vector_t *v, void *elem);

PCL_EXPORT pcl_vector_t *pcl_vector_sort(pcl_vector_t *v, int (*cmp)(const void *, const void *));

#ifdef __cplusplus
}
#endif

#endif

