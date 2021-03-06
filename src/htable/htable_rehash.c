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
	int new_capacity = ipcl_htable_chkcapacity(
		grow ? (uint64_t) ht->capacity << 1U : (uint64_t) ht->capacity >> 1U);

	int new_table_mask = new_capacity - 1;

	if(new_capacity < 0)
		return TRC();

	int *new_entry_lookup;
	pcl_htable_entry_t *new_entries;
	ipcl_htable_init(new_capacity, &new_entries, &new_entry_lookup);

	/* recompute all non-deleted entries */
	for(int count = 0, i = 0; i < ht->count_used; i++)
	{
		pcl_htable_entry_t *oldent = &ht->entries[i];

		/* skip deleted slots */
		if(!oldent->key)
			continue;

		pcl_htable_entry_t *ent = &new_entries[count];
		ent->key = oldent->key;
		ent->value = oldent->value;
		ent->code = oldent->code;

		int hashidx = (int) (ent->code & new_table_mask);
		int entidx = new_entry_lookup[hashidx];

		if(entidx != -1)
		{
			pcl_htable_entry_t *head = &new_entries[entidx];
			ent->next = head->next;
			head->next = count;
		}
		else
		{
			ent->next = -1;
			new_entry_lookup[hashidx] = count;
		}

		count++;
	}

	/* frees entry_lookup[] as well */
	pcl_free(ht->entries);

	ht->capacity = new_capacity;
	ht->table_mask = new_table_mask;
	ht->entries = new_entries;
	ht->entry_lookup = new_entry_lookup;
	ht->count_used = ht->count; // reset count_used, we defrag'd entries array

	return 0;
}
