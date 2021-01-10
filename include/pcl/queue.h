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

#ifndef LIBPCL_QUEUE_H
#define LIBPCL_QUEUE_H

/** @defgroup queue Queue
 * A FIFO data structure.
 * @{
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Create a queue object.
 * @param cleanup optional cleanup handler for queue items
 * @return pointer to a new queue object that must be freed via ::pcl_queue_free
 */
PCL_EXPORT pcl_queue_t *pcl_queue(pcl_cleanup_t cleanup);

/** Get the size (number of items) of a queue.
 * @param q pointer to a queue object
 * @return number of items in the queue
 */
PCL_EXPORT int pcl_queue_size(pcl_queue_t *q);

/** Indicates if a queue is empty.
 * @param q pointer to a queue object
 * @return true if empty and false otherwise
 */
PCL_EXPORT bool pcl_queue_empty(pcl_queue_t *q);

/** Enqueues an item.
 * @param q pointer to a queue object
 * @param item pointer to an item to enqueue
 */
PCL_EXPORT void pcl_queue_add(pcl_queue_t *q, void *item);

/** Retrieve the item at the head of a queue without removing it.
 * @param q pointer to a queue object
 * @return pointer to an item or \c NULL if queue is empty. To avoid ambiguity, do not enqueue
 * \c NULL items.
 */
PCL_EXPORT void *pcl_queue_peek(pcl_queue_t *q);

/** Dequeue an item.
 * @note caller repsonsible for releasing any resources used by returned item
 * @param q pointer to a queue object
 * @return pointer to an item or \c NULL if queue is empty. To avoid ambiguity, do not enqueue
 * \c NULL items.
 */
PCL_EXPORT void *pcl_queue_remove(pcl_queue_t *q);

/** Remove all items from a queue.
 * @note if the queue is not empty, this will call the optional ::pcl_cleanup_t for each item.
 * @param q pointer to a queue object
 */
PCL_EXPORT void pcl_queue_clear(pcl_queue_t *q);

/** Release all resources used by a queue.
 * @note if the queue is not empty, this will call the optional ::pcl_cleanup_t for each item.
 * @param q pointer to a queue object
 */
PCL_EXPORT void pcl_queue_free(pcl_queue_t *q);

#ifdef __cplusplus
}
#endif

/** @} */

#endif // LIBPCL_QUEUE_H
