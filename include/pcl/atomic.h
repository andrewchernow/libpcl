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

#ifndef LIBPCL_ATOMIC_H__
#define LIBPCL_ATOMIC_H__

/** @defgroup atomic Atomic Operations
 * Perform atomic operations. These functions are useful for updating or fetching a value
 * in an atomic fashion: such as a global variable in a multi-threaded application.
 * Atomic operations, where applicable, are a much faster synchronization technique than Mutexes.
 * @{
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Performs an atomic exchange operation.
 * @param dest pointer to an atomic value that is set to \a val
 * @param val new atomic value for \a dest
 * @return initial value of \a dest
 */
PCL_PUBLIC pcl_atomic_t pcl_atomic_exchange(pcl_atomic_t *dest, pcl_atomic_t val);

/** Performs an add and fetch operation.
 * @param dest pointer to an atomic value
 * @param add value to add to \a dest, which can be negative to subtract
 * @return the result of the addition operation
 */
PCL_PUBLIC pcl_atomic_t pcl_atomic_add_fetch(pcl_atomic_t *dest, pcl_atomic_t add);

/** Performs a fetch and add.
 * @param dest pointer to an atomic value
 * @param add value to add to \a dest, which can be negative to subtract
 * @return initial value of \a dest
 */
PCL_PUBLIC pcl_atomic_t pcl_atomic_fetch_add(pcl_atomic_t *dest, pcl_atomic_t add);

/** Performs a compare and exhange operation.
 * @param dest pointer to an atomic value
 * @param expected a value to compare with \a dest
 * @param desired value to be written to \a dest if \a dest and \a expected are equal
 * @return initial value of \a dest
 */
PCL_PUBLIC pcl_atomic_t pcl_atomic_compare_exchange(pcl_atomic_t *dest, pcl_atomic_t expected,
	pcl_atomic_t desired);

/** Performs an atomic fetch (load) operation.
 * @param dest pointer to an atomic
 * @return value of \a dest
 */
PCL_PUBLIC pcl_atomic_t pcl_atomic_fetch(pcl_atomic_t *dest);

#ifdef __cplusplus
}
#endif

/** @} */
#endif //LIBPCL_ATOMIC_H__
