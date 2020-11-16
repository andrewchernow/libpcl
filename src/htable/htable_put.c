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

#include "_htable.h"
#include <pcl/error.h>
#include <pcl/stdlib.h>

int
pcl_htable_put(pcl_htable_t *ht, const char *key, void *value, bool unique)
{
	/* NULL values allowed */
	if(!(ht && key))
		return BADARG();

	int index;
	uintptr_t code;
	pcl_htable_entry_t *e = ipcl_htable_lookup(ht, key, &code, &index);

	if(e)
	{
		if(unique)
			return SETERR(PCL_EEXIST);

		const char *old_key = e->key;
		void *old_value = e->value;

		e->key = key;
		e->value = value;

		/* can't use ipcl_htable_remove_entry since that will free our entry object. In this case,
		 * we can reuse the entry object, just allow the user to free the old key & value.
		 */
		if(ht->remove_entry)
			ht->remove_entry(old_key, old_value, ht->userp);
	}
	/* doesn't exist, add it */
	else
	{
		/* check if a rehash is needed. The rehash function only returns an error if the
		 * capacity has exceeded the maximum size for the architecture. Sets PCL_ERANGE.
		 */
		if(ht->count > (int) (ht->max_loadfac * (float) ht->capacity) && ipcl_htable_rehash(ht, true))
			return TRC();

		pcl_htable_entry_t *ent = pcl_malloc(sizeof(pcl_htable_entry_t));

		ent->key = key;
		ent->value = value;
		ent->code = code;

		/* add new entry to collision list */
		pcl_htable_entry_t *head = ht->entries[index];
		ht->entries[index] = ent;
		ent->next = head;
		ht->count++;
	}

	return 0;
}
