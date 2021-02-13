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

#include "_htable.h"
#include <pcl/error.h>

int
ipcl_htable_chkcapacity(uint64_t capacity)
{
	capacity = max(capacity, MINTBLSIZE);

	/* if not a power of 2, round up to next power of 2 */
	if((capacity & (capacity - 1)) != 0)
	{
		uint64_t x = capacity;

		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;

		capacity = x + 1;
	}

#ifndef PCL_64BIT
	/* 32-bit machines are limited to 1<<25 (33,554,432) buckets. Each entry is 16 bytes so
	 * the table will be using 500M (536,870,912 bytes) at this number of buckets. This doesn't
	 * account for entry data (keys + values). Also, the current table size is 1<<24 (16,777,216)
	 * which means there are 16,777,216 * .75 (default loadfac) entries (12,582,912). If each
	 * entry's data (key + value) is 32 bytes (kind of low), then another 384M is used for entry
	 * data; making a table of 1<<24 256M + 384M or 640M. Growing the table to 1<<25 adds another
	 * 256M in buckets totaling 896M. As entries are added, the number grows from here. If we
	 * allowed 1<<26, the table size alone would require 1G of memory (no entry data). Thus, 1<<25
	 * was chosen as the maximum on 32-bit machines. Also considered was whatever the operating
	 * system already has in use. It is very unlikely there is anywhere near 2-4G of RAM for a
	 * hash table ... upgrade to a 64-bit machine.
	 */
	if(capacity > (1U << 25))
		return SETERR(PCL_ERANGE);
#else
	/* 64-bit machines allow for 1<<30 (1,073,741,824) buckets. Entries are 32 bytes so right
	 * away 32G of memory is required for a capacity of 1<<30. Adding in entry data, this maximum
	 * value is plenty. Even machines with 64G of memory would probably barf on a table size
	 * of 1<<30: accounting for entry data + operating system.
	 */
	if(capacity > (1U << 30))
		return SETERR(PCL_ERANGE);
#endif

	return (int) capacity;
}

