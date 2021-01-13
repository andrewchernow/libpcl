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
pcl_htable_remove(pcl_htable_t *ht, const void *key)
{
	if(!(ht && key))
		return BADARG();

	uintptr_t code = ht->hashcode(key, ht->key_len);
	int hashidx = (int) (code % ht->capacity);
	int entidx = ht->hashidx[hashidx];
	pcl_htable_entry_t *prev = NULL;

	while(entidx != -1)
	{
		pcl_htable_entry_t *ent = &ht->entries[entidx];

		if(ent->code == code && ht->key_equals(ent->key, key, ht->key_len))
		{
			if(prev)
				prev->next = ent->next;
			else
				ht->hashidx[hashidx] = ent->next;

			if(ht->remove_entry)
				ht->remove_entry(ent->key, ent->value);

			ent->next = -1;
			ent->code = 0;
			ent->key = ent->value = NULL;

			ht->count--;

			if(ht->capacity != MINTBLSIZE && ht->count < (int) (ht->min_loadfac * (float) ht->capacity))
				if(ipcl_htable_rehash(ht, false) < 0)
					return TRC();

			break;
		}

		prev = ent;
		entidx = ent->next;
	}

	return ht->count;
}
