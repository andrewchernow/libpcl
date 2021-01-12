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

#ifndef LIBPCL_ARRAY_H
#define LIBPCL_ARRAY_H

/** @defgroup array Array Management
 * An array is a pointer table of allocated elements. This differs from a PCL vector in that
 * there is no concept of element size. The elements are user allocated pointers that can
 * all be the same type of object, different objects or even \c NULL.
 *
 * It is common to interact (read) with an array's \a elements directly, while this is cumbersome
 * with a vector. Although a vector can be used like a pointer table, it is a bit awkward because
 * elements must be added as pointers to pointers: element size would be \c sizeof(void*). Thus,
 * when getting an element from a vector, one must dereference the element value:
 * `*(void **) pcl_vector_get(v, 0)`.
 *
 * ### Create argv array
 * @code
 * // create an argv array of 4 arguments with a trailing NULL
 * pcl_array_t *arr = pcl_array(5, pcl_cleanup_ptr);
 *
 * pcl_array_push(arr, strdup("-a"));
 * pcl_array_push(arr, strdup("--quiet"));
 * pcl_array_push(arr, strdup("--file=\"example/file.txt\""));
 * pcl_array_push(arr, strdup("-t"));
 *
 * // replace element 0 with -b, cleanup handler called for element 0
 * if(something_is_true)
 *   pcl_array_set(arr, strdup("-b"), 0);
 *
 * // remove quiet if noisey is true, internally does a cleanup and memmove
 * if(noisey)
 *   pcl_array_remove(arr, 1);
 *
 * // note, any unset element defaults to NULL.
 * execv("some_program", arr->elements);
 *
 * pcl_array_free(arr);
 * @endcode
 *
 * @{
 */
#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_array
{
	/** number of elements in the array */
	int count;
	/** internal table size, number of elements */
	int capacity;
	/** pointer table of elements. */
	void **elements;
	pcl_cleanup_t cleanup;
};

/** Create an array.
 * @param initial_capacity initial size of array
 * @param cleanup pointer to a cleanup routine.
 * @return pointer to a new array object
 * @see pcl_cleanup_ptr
 */
PCL_EXPORT pcl_array_t *pcl_array(int initial_capacity, pcl_cleanup_t cleanup);

/** Get an element from an array.
 * This is typically not used when iterating over an array, as one can just directly access
 * the element via the \a elements member. However, this is useful for cases when not looping
 * an array as it performs bounds checking of index.
 * @param arr pointer to an array object
 * @param index element index
 * @return pointer to an element or \c NULL on error. To distinguish between an error and a
 * \c NULL element, check ::pcl_errno. It will be ::PCL_EOKAY if a \c NULL element.
 */
PCL_EXPORT void *pcl_array_get(pcl_array_t *arr, int index);

/** Set an element of an array, replacing a possible existing element. When setting elements,
 * the array's \a count is not consulted. Instead, this allows a set operation as long as
 * \a index is less than \a capacity. This will set \c count to `index + 1` if \a count is
 * less than `index + 1`. This is very different than ::pcl_array_insert or ::pcl_array_push,
 * which always inserts an element, bound to array's current \c count, and growing the array if required.
 * When performing a set operation, the array is never grown. Thus, trying to set an element at
 * an \a index greater than or equal to \a capacity will throw a ::PCL_EINDEX error.
 *
 * If an element exists at \a index, it is removed and passed to the cleanup handler if both the
 * cleanup handler and element are not \c NULL.
 *
 * @param arr arr pointer to an array object
 * @param elem pointer to an element
 * @param index element index
 * @return 0 for success and -1 on error.
 * @see pcl_array_push
 */
PCL_EXPORT int pcl_array_set(pcl_array_t *arr, void *elem, int index);

/** Insert an element.
 * @param arr pointer to an array object
 * @param elem pointer to an element
 * @param index the insertion index. this can be equal to the array's current count, in which
 * case \a elem is appended
 * @return the new element count or -1 on error.
 * @see pcl_array_set
 */
PCL_EXPORT int pcl_array_insert(pcl_array_t *arr, void *elem, int index);

/** Remove an element from an array. This will call the cleanup handler if both the cleanup
 * handler and element at \a index are not \c NULL.
 *
 * @param arr pointer to an array object
 * @param index element index
 * @return the new count of element or -1 on error.
 */
PCL_EXPORT int pcl_array_remove(pcl_array_t *arr, int index);

/** Free an array. This will call the cleanup handler, if not \c NULL, for all elements that
 * are not \c NULL and then free the array object.
 * @param arr pointer to an array object
 */
PCL_EXPORT void pcl_array_free(pcl_array_t *arr);

PCL_INLINE int
pcl_array_push(pcl_array_t *arr, void *elem)
{
	return pcl_array_insert(arr, elem, arr ? arr->count : 0);
}

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_ARRAY_H
