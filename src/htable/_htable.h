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

#ifndef LIBPCL__HTABLE_H
#define LIBPCL__HTABLE_H

#include <pcl/htable.h>

#define MAX_LOADFAC 0.75f
#define MIN_LOADFAC 0.2f
#define MINTBLSIZE 17

#ifdef __cplusplus
extern "C" {
#endif

int ipcl_htable_capacity(int capacity);

pcl_htable_entry_t *ipcl_htable_lookup(const pcl_htable_t *ht, const void *key,
	uintptr_t *codep, int *hashidxp);

/**
 *
 * @param ht
 * @param grow true to grow table and false to shrink. When true, the new table size is set
 * to double its current value and then rounded up to the nearest prime. When false, the
 * table size is divided in half and then rounded up to the nearest table size.
 * @return
 */
int ipcl_htable_rehash(pcl_htable_t *ht, bool grow);

void ipcl_htable_init(int capacity, pcl_htable_entry_t **entries, int **hashidx);

#ifdef __cplusplus
}
#endif

#endif // LIBPCL__HTABLE_H
