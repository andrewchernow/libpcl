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

#ifndef LIBPCL_STACK_H
#define LIBPCL_STACK_H

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Create a stack object.
 * @param cleanup optional item cleanup handler
 * @return pointer to a stack object or \c NULL on error
 * @see pcl_cleanup_ptr
 */
PCL_EXPORT pcl_stack_t *pcl_stack(pcl_cleanup_t cleanup);

/** Get the size (number of items) of a stack object.
 * @param s pointer to a stack object
 * @return number of stack items
 */
PCL_EXPORT int pcl_stack_size(pcl_stack_t *s);

/** Indicates if a stack is empty.
 * @param s pointer to a stack object
 * @return
 */
PCL_EXPORT bool pcl_stack_empty(pcl_stack_t *s);

/** Add an item to the top of the stack.
 * @param s pointer to a stack object
 * @param item
 */
PCL_EXPORT void pcl_stack_push(pcl_stack_t *s, void *item);

/** Retrievve an item from the top of the stack without removing it.
 * @param s pointer to a stack object
 * @return
 */
PCL_EXPORT void *pcl_stack_peek(pcl_stack_t *s);

/** Retrieve and remove an item from the top of the stack.
 * @note it is the caller's responsibility to free the returned item.
 * @param s pointer to a stack object
 * @return
 */
PCL_EXPORT void *pcl_stack_pop(pcl_stack_t *s);

/** Remove all items from a stack. This will call the optional cleanup handler.
 * @param s pointer to a stack object
 */
PCL_EXPORT void pcl_stack_clear(pcl_stack_t *s);

/** Release all resources used by a stack object.
 * @param s pointer to a stack object
 */
PCL_EXPORT void pcl_stack_free(pcl_stack_t *s);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL_STACK_H
