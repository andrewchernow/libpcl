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

pcl_htable_entry_t *
pcl_htable_lookup(const pcl_htable_t *ht, const void *key)
{
	if(!ht || !key)
		return R_SETERR(NULL, PCL_EINVAL);

	pcl_htable_entry_t *e = ipcl_htable_lookup(ht, key, NULL, NULL);

	return e ? e : R_TRC(NULL);
}

pcl_htable_entry_t *
ipcl_htable_lookup(const pcl_htable_t *ht, const void *key, uintptr_t *codep, int *hashidxp)
{
	uintptr_t code = ht->hashcode(key, ht->key_len);

	if(codep)
		*codep = code;

	int hashidx = (int) (code & ht->table_mask);

	if(hashidxp)
		*hashidxp = hashidx;

	int entidx = ht->entry_lookup[hashidx];

	while(entidx != -1)
	{
		pcl_htable_entry_t *e = &ht->entries[entidx];

		if(e->code == code && ht->key_equals(e->key, key, ht->key_len))
			return e;

		entidx = e->next;
	}

	return R_SETERR(NULL, PCL_ENOTFOUND);
}
