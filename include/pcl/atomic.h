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

#ifndef LIBPCL_ATOMIC_H__
#define LIBPCL_ATOMIC_H__

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Sets *dest to val and returns the initial contents of *dest. */
PCL_EXPORT pcl_atomic_t
pcl_atomic_exchange(pcl_atomic_t *dest, pcl_atomic_t val);

/* Adds 'add' to *dest and returns the result of the operation.
 * Set 'add' to a negative number to subtract from *dest.
 */
PCL_EXPORT pcl_atomic_t
pcl_atomic_add_fetch(pcl_atomic_t *dest, pcl_atomic_t add);

/* Adds 'add' to *value and returns the initial value of *dest.
 * Set 'add' to a negative number to subtract from *dest: returns new 'value'
 */
PCL_EXPORT pcl_atomic_t
pcl_atomic_fetch_add(pcl_atomic_t *dest, pcl_atomic_t add);

/* Compares the value of '*dest' with 'expected' and if equal, writes 'desired'
 * to '*dest'. If not equal, no operation/write is performed. Returns the
 * contents of *dest in either case.
 */
PCL_EXPORT pcl_atomic_t
pcl_atomic_compare_exchange(pcl_atomic_t *dest, pcl_atomic_t expected, pcl_atomic_t desired);

/* Reads and returns the current value of *dest */
PCL_EXPORT pcl_atomic_t
pcl_atomic_fetch(pcl_atomic_t *dest);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_ATOMIC_H__
