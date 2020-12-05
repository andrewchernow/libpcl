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

#ifndef LIBPCL_FARMHASH_H
#define LIBPCL_FARMHASH_H

/** @defgroup farmhash FarmHash Hashing Algorithm
 * This is Google's FarmHash Library: https://github.com/google/farmhash. It is designed for
 * use with hash tables and is the default hash code function for ::pcl_htable_t.
 * @{
 */

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Get a 32-bit farmhash hash code.
* @param data pointer to a NUL-terminated string to hash
* @return 32-bit hash code
*/
PCL_EXPORT uint64_t pcl_farmhash32(const char *data);

/** Get a 32-bit farmhash hash code.
 * @param data pointer to a NUL-terminated string to hash
 * @param len  byte length of \a data
 * @param seed 32-bit seed value
 * @return 32-bit hash code
 */
PCL_EXPORT uint64_t pcl_farmhash32_seed(const void *data, size_t len, uint32_t seed);

/** Get a 64-bit farmhash hash code.
 * @param data pointer to a NUL-terminated string to hash
 * @return 64-bit hash code
 */
PCL_EXPORT uint64_t pcl_farmhash64(const char *data);

/** Get a 64-bit farmhash hash code.
 * @param data pointer to a NUL-terminated string to hash
 * @param len  byte length of \a data
 * @param seed 64-bit seed value
 * @return 64-bit hash code
 */
PCL_EXPORT uint64_t pcl_farmhash64_seed(const void *data, size_t len, uint64_t seed);

/** Get a 128-bit farmhash hash code.
 * @param data pointer to a NUL-terminated string to hash
 * @return 128-bit hash code
 */
PCL_EXPORT uint128_t pcl_farmhash128(const char *data);

/** Get a 128-bit farmhash hash code.
 * @param data pointer to a NUL-terminated string to hash
 * @param len  byte length of \a data
 * @param seed 128-bit seed value
 * @return 128-bit hash code
 */
PCL_EXPORT uint128_t pcl_farmhash128_seed(const void *data, size_t len, uint128_t seed);

#ifdef __cplusplus
}
#endif

/** @} */
#endif // LIBPCL_FARMHASH_H
