
/*
	Portable C Library ("PCL")
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

#ifndef LIBPCL_VECTOR_H
#define LIBPCL_VECTOR_H

/** @defgroup vector Vector
 * A PCL Vector is a dynamic and contiguous block of memory for an array of same-sized elements.
 * The vector automatically expands this block of memory if an insertion would exceed
 * the vector's current capacity. The vector never automatically shrinks itself during an element
 * removal, but a vector can be shrunk using ::pcl_vector_compact.
 *
 * Although a vector can store strings, it is adviceable to use a ::pcl_array_t instead. When
 * storing strings (or any pointer) as elements, a pointer to the element pointer must
 * be stored within the vector, requiring a dereference when the element is retrieved:
 * `*(void **) pcl_vector_get(v, 0)`. This is not what the vector was designed for even though
 * it will work.
 *
 * #### Basic Usage
 * @code
 * #include <pcl/init.h>
 * #include <pcl/vector.h>
 * #include <stdio.h>
 *
 * int main(int argc, char **argv)
 * {
 *   // always first
 *   pcl_init();
 *
 *   // cleanup handler can be NULL, integer doesn't need it
 *   pcl_vector_t *vec = pcl_vector(8, sizeof(int), NULL);
 *
 *   for(int i=0; i < vec->capacity * 2; i++)
 *     pcl_vector_append(vec, &i); // vector will have to expand after 8 appends
 *
 *   for(int i=0; i < vec->count; i++)
 *   {
 *     int *elem = pcl_vector_get(vec, i);
 *     printf("%d\n", *elem); // 0\n1\n...15\n
 *   }
 *
 *   pcl_vector_free(vec);
 * }
 * @endcode
 * @{
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Vector object. Used to track elements and grow the internal elements array when required. */
struct tag_pcl_vector
{
	/** number of elements in vector */
	int count;

	/** number of elements allocated */
	int capacity;

	/** byte size of an element */
	size_t size;

	/** Element array. This is always pcl_vector_t.capacity \c * pcl_vector_t.size bytes */
	char *elems;

	/** Element cleanup callback.
	 * @warning DO NOT set this to ::pcl_cleanup_ptr. That function frees the element
	 * argument but vector elements are owned by the vector. A vector cleanup function should
 	 * free allocated memory contained within the element: like a struct member.
 	 * ### Example
 	 * @code
 	 * static void mycleanup(void *elem)
 	 * {
 	 *   myobj_t *o = elem; // NEVER free 'elem', owned by vector
 	 *   pcl_free(o->str);
 	 * }
 	 * @endcode
	 * */
	pcl_cleanup_t cleanup;
};

/** Get an element.
 * @warning this provides no bounds checking on \a pos and is designed to be used in cases
 * where \a pos is known to be valid: such as a loop constrained by the vector's count. This
 * is an optimization for the most common usage pattern. Expect a core dump or undefined
 * behavior if \a v or \a pos is invalid.
 * @param v pointer to a vector
 * @param pos zero-based position of element to retreive
 * @return pointer to element at \a pos
 * @see pcl_vector_get_safe
 */
#define pcl_vector_get(v, pos) \
  ((void *)(((pcl_vector_t *)(v))->elems + ((pos) * ((pcl_vector_t *)(v))->size)))

/** Creates a new vector object.
 * @param capacity initial capacity of the vector as number of elements
 * @param elemsize byte size of each element
 * @param cleanup element cleanup handler called whenever an element is removed from the vector.
 * @return an allocated vector object that must be freed by the application. It is an error
 * to provide ::pcl_cleanup_ptr for \a cleanup.
 * @see pcl_vector_free
 */
PCL_PUBLIC pcl_vector_t *pcl_vector(int capacity, size_t elemsize, pcl_cleanup_t cleanup);

/** Insert an element. To append an element, set \a pos to pcl_vector_t.count
 * @note Starting at \a elem, pcl_vector_t.size bytes are copied into the vector's internal
 * memory.
 * @param v pointer to a vector
 * @param elem pointer to an element. If this is \c NULL, a zero'd element is inserted
 * @param pos the 0-based position to insert the new element. If this is equal to the
 * pcl_vector_t.count, the element will be appended.
 * @return pointer to the element inserted, which is not the same as \a elem. \c NULL is
 * returned on error.
 */
PCL_PUBLIC void *pcl_vector_insert(pcl_vector_t *v, const void *elem, int pos);

/** Remove an element.
 * @param v pointer to a vector
 * @param pos a zero-based position of the element to remove
 * @return 0 for success and -1 on error.
 */
PCL_PUBLIC int pcl_vector_remove(pcl_vector_t *v, int pos);

/** Get an element with bounds checking (safe version of ::pcl_vector_get).
 * @param v pointer to a vector
 * @param pos zero-based position of element to get
 * @return pointer to the element or \c NULL on error
 * @see pcl_vector_get
 */
PCL_PUBLIC void *pcl_vector_get_safe(pcl_vector_t *v, int pos);

/** Compact a vector such that its element capacity matches its element count.
 * @param v pointer to a vector
 * @return pointer to \a v argument
 */
PCL_PUBLIC pcl_vector_t *pcl_vector_compact(pcl_vector_t *v);

/** Clear/remove all elements from a vector. This doesn't adjust the vector's capacity. To
 * adjust its capacity, call ::pcl_vector_compact after this.
 * @param v pointer to a vector
 * @return pointer to the \a v argument
 * @see pcl_vector_compact
 */
PCL_PUBLIC pcl_vector_t *pcl_vector_clear(pcl_vector_t *v);

/** Release all resources used by a vector as well as the vector itself.
 * @param v pointer to a vector that is unusable after this call. If this is \c NULL, the
 * function silently ignores the request.
 * @see pcl_vector
 */
PCL_PUBLIC void pcl_vector_free(pcl_vector_t *v);

/** Sort a vector using \c qsort.
 * @param v ppointer to a vector
 * @param compare pointer to a function comparator whose behavior is identical to \c qsort
 * @return pointer to the \a v argument
 */
PCL_PUBLIC pcl_vector_t *pcl_vector_sort(pcl_vector_t *v, pcl_compare_t compare);

/** Append an element.
 * @param v pointer to a vector
 * @param elem pointer to an element
 * @return pointer to the vector element inserted or \c NULL on error
 */
PCL_INLINE void *
pcl_vector_append(pcl_vector_t *v, void *elem)
{
	return pcl_vector_insert(v, elem, v ? v->count : 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif

