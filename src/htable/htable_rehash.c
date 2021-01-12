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
#include <pcl/alloc.h>
#include <pcl/error.h>
#include <string.h>

int
ipcl_htable_rehash(pcl_htable_t *ht, bool grow)
{
	int new_capacity = ipcl_htable_capacity(grow ? ht->capacity * 2 : ht->capacity / 2);

	if(new_capacity < 0)
		return TRC();

	int *new_hashidx;
	pcl_htable_entry_t *new_entries;
	ipcl_htable_init(new_capacity, &new_entries, &new_hashidx);

	/* recompute all non-deleted entries */
	for(int count = 0, i = 0; i < ht->usedCount; i++)
	{
		pcl_htable_entry_t *oldent = &ht->entries[i];

		/* skip deleted slots */
		if(!oldent->key)
			continue;

		pcl_htable_entry_t *ent = &new_entries[count];
		ent->key = oldent->key;
		ent->value = oldent->value;
		ent->code = oldent->code;

		int hashidx = (int) (ent->code % new_capacity);
		int entidx = new_hashidx[hashidx];

		if(entidx != -1)
		{
			pcl_htable_entry_t *head = &new_entries[entidx];
			ent->next = head->next;
			head->next = count;
		}
		else
		{
			ent->next = -1;
			new_hashidx[hashidx] = count;
		}

		count++;
	}

	/* frees hashidx[] as well */
	pcl_free_safe(ht->entries);

	ht->capacity = new_capacity;
	ht->entries = new_entries;
	ht->hashidx = new_hashidx;
	ht->usedCount = ht->count; // reset usedCount, we defrag'd entries array

	return 0;
}
