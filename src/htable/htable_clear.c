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
#include <pcl/alloc.h>
#include <string.h>

void
pcl_htable_clear(pcl_htable_t *ht, bool shrink)
{
	if(!ht)
		return;

	for(int i = 0; i < ht->capacity; i++)
	{
		pcl_htable_entry_t *e = ht->entries[i];

		if(e)
		{
			pcl_htable_entry_t *next;

			do
			{
				next = e->next;
				ipcl_htable_remove_entry(ht, e, false); // set allow_rehash to false, about to shrink
			}
			while((e = next));

			ht->entries[i] = NULL;
		}
	}

	/* internally, shrink is only set to false by pcl_htable_free */
	if(shrink)
	{
		ht->capacity = MIN_TABLE_SIZE;
		ht->entries = pcl_realloc(ht->entries, sizeof(void*) * ht->capacity);
		memset(ht->entries, 0, sizeof(void*) * ht->capacity);
	}
}
