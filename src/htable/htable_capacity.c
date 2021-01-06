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

#include "_htable.h"
#include <pcl/error.h>

int
ipcl_htable_capacity(int capacity)
{
	/* ~50 million max capacity for 32-bit machines and ~1.6 billion max capacity on
	 * 64-bit machines. This is plenty for both architectures and the result of using the
	 * max capacity on either architecture is a significant amount of memory. 64-bit could
	 * address more, but a table capacity of 1.6 billion (~1.2 billion entries considering
	 * load factor) is an insanely sized hash table.
	 */
	static int prime_table[] = {
		MIN_TABLE_SIZE, 31, 53, 97, 193, 389, 769, 1543, 3079, 6151,
		12289, 24593, 49157, 98317, 196613, 393241, 786433, 1572869,
		3145739, 6291469, 12582917, 25165843, 50331653
#ifdef PCL_64BIT
		, 100663319, 201326611 ,402653189, 805306457, 1610612741
#endif
	};

	for(int i=0; i < countof(prime_table); i++)
		if(capacity < prime_table[i])
			return prime_table[i];

	return SETERR(PCL_ERANGE);
}

