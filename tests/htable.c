/*
  Portable C Library (PCL)
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

#include "test.h"
#include <pcl/htable.h>
#include <pcl/error.h>
#include <pcl/array.h>
#include <string.h>

typedef struct
{
	char *name;
	int age;
} person_t;

#define NUM_PEOPLE (int) (sizeof(people) / sizeof(people[0]))

person_t people[] = {
	{"Andrew", 24}, {"Joan", 32}, {"Henry", 81}, {"Marsha", 17}, {"Linda", 32},
	{"Michael", 67}, {"Samuel", 53}, {"Fred", 29}, {"Eric", 38}, {"Emily", 63},
	{"Adam", 30}, {"David", 31}, {"Carolyn", 78}, {"Jessica", 18}, {"George", 54},
	{"James", 19}, {"Heather", 87}, {"Simon", 91}, {"Terry", 71}, {"Jack", 8},
	{"John", 41}, {"Sil", 32}, {"Sharon", 28}, {"Sherry", 64}, {"Brian", 56}
};

/**$ Test rehashing table: grow and shrink */
TESTCASE(htable_rehash)
{
	/* see src/htable/htable_capacity.c for prime table */
	pcl_htable_t *ht = pcl_htable(0);

	ASSERT_NOTNULL(ht, "faield to create hash table");
	ASSERT_INTEQ(ht->capacity, 17, "wrong initial table size");

	/* these are the defaults NOW, but if they change this test would break.
	 * So manually set them.
	 */
	ht->min_loadfac = .20f;
	ht->max_loadfac = .75f;

	/* capacity of 0 means it will be initially set to 17. max_loadfac is .75 so a rehash
	 * should not occur until there are more than 12 entries (17 * .75).
	 */
	for(int i = 0; i < 12; i++)
	{
		person_t *p = &people[i];
		ASSERT_INTEQ(pcl_htable_put(ht, p->name, p, true), 0, "failed to put entry");
	}

	ASSERT_INTEQ(ht->capacity, 17, "unexpected rehash occurred");
	ASSERT_INTEQ(ht->count, 12, "wrong entry count");

	/* Putting one more entry should cause a rehash */
	int r = pcl_htable_put(ht, people[12].name, &people[12], true);
	ASSERT_INTEQ(r, 0, "failed to put entry");
	ASSERT_INTEQ(ht->count, 13, "wrong entry count");
	ASSERT_INTEQ(ht->capacity, 53, "expected rehash did not occur");

	/* test a rehash that shrinks the table. 53 * .2 = 10 so we need to remove
	 * 4 before a rehash will occur that shrimks the table. table size should be 31.
	 */
	for(int i = 0; i < 4; i++)
	{
		ASSERT_INTEQ(pcl_htable_remove(ht, people[i].name), 13 - i - 1, "wrong return value for remove");

		/* rehash should occur after removing 4 entries, table size will be 31 */
		if(i == 3 && ht->count == 9)
			ASSERT_INTEQ(ht->capacity, 31, "expected shrink rehash");
		else
			ASSERT_INTEQ(ht->capacity, 53, "unexpected rehash occurred");
	}

	pcl_htable_free(ht);
	return true;
}

/**$ Get entries from a table */
TESTCASE(htable_get)
{
	pcl_htable_t *ht = pcl_htable(0);

	for(int i = 0; i < NUM_PEOPLE; i++)
	{
		person_t *p = &people[i];
		ASSERT_INTEQ(pcl_htable_put(ht, p->name, p, true), 0, "failed to put entry");
	}

	person_t *p = pcl_htable_get(ht, "Fred");
	ASSERT_NOTNULL(p, "failed to get entry");
	ASSERT_INTEQ(p->age, 29, "wrong value for returned entry");

	p = pcl_htable_get(ht, "Sherry");
	ASSERT_NOTNULL(p, "failed to get entry");
	ASSERT_INTEQ(p->age, 64, "wrong value for returned entry");

	pcl_htable_entry_t *ent = pcl_htable_lookup(ht, "David");
	ASSERT_NOTNULL(ent, "failed to get entry");
	ASSERT_STREQ(ent->key, "David", "wrong entry returned by lookup");

	p = ent->value;
	ASSERT_INTEQ(p->age, 31, "wrong value for returned entry");

	pcl_htable_free(ht);
	return true;
}

/**$ Putting unique key exists error */
TESTCASE(htable_keyexists)
{
	pcl_htable_t *ht = pcl_htable(0);
	ASSERT_INTEQ(pcl_htable_put(ht, "key", NULL, true), 0, "wrong return value for put");
	ASSERT_INTEQ(pcl_htable_put(ht, "key", NULL, true), -1, "duplicate entry didn't fail");
	ASSERT_INTEQ(pcl_errno, PCL_EEXIST, "wrong pcl error set expected PCL_EEXIST");
	pcl_htable_free(ht);
	return true;
}

/**$ NULL value handling */
TESTCASE(htable_nullvalue)
{
	pcl_htable_t *ht = pcl_htable(0);
	ASSERT_INTEQ(pcl_htable_put(ht, "key", NULL, true), 0, "wrong return value for put");

	void *value = pcl_htable_get(ht, "key");
	ASSERT_NULL(value, "wrong return value from get");
	ASSERT_INTEQ(pcl_errno, PCL_EOKAY, "wrong pcl error set expected PCL_EOKAY");

	pcl_htable_free(ht);
	return true;
}

/**$ Get an array of keys */
TESTCASE(htable_keys)
{
	pcl_htable_t *ht = pcl_htable(0);

	for(int i = 0; i < NUM_PEOPLE; i++)
	{
		person_t *p = &people[i];
		ASSERT_INTEQ(pcl_htable_put(ht, p->name, p, true), 0, "failed to put entry");
	}

	pcl_array_t *keys = pcl_htable_keys(ht);
	ASSERT_NOTNULL(keys, "htable_keys failed");
	ASSERT_NULL((void *)(uintptr_t) keys->cleanup, "cleanup handler has been set");
	ASSERT_INTEQ(keys->count, NUM_PEOPLE, "wrong number of keys");

	/* make sure all keys can be found in people[] */
	for(int i = 0; i < NUM_PEOPLE; i++)
	{
		int k = 0;
		person_t *p = &people[i];

		for(; k < keys->count; k++)
			if(strcmp(p->name, keys->elements[k]) == 0)
				break;

		/* NOTE: if k == count, key was not found */
		ASSERT_INTNEQ(k, keys->count, "could not find name within keys");

		/* do a lookup, sanity check */
		ASSERT_NOTNULL(pcl_htable_lookup(ht, keys->elements[k]), "key not found within table");
	}

	pcl_array_free(keys);
	pcl_htable_free(ht);
	return true;
}

/**$ Add duplicate keys to table (replace operation) */
TESTCASE(htable_dupkeys)
{
	pcl_htable_t *ht = pcl_htable(0);
	ASSERT_INTEQ(pcl_htable_put(ht, "key", &people[0], false), 0, "wrong return value for put");
	ASSERT_INTEQ(pcl_htable_put(ht, "key", &people[1], false), 0, "wrong return value for put");

	/* Duplicate keys are a replace operation. The PCL htable doesn't support duplicate keys.
	 * The entry structure is preserved but the key and value are passed to optional remove_entry
	 * handler. The new key and value are assigned to the preserved entry structure. Thus, looking
	 * up key should result in a collision list with a single entry (head of list).
	 */

	pcl_htable_entry_t *ent = pcl_htable_lookup(ht, "key");
	ASSERT_NOTNULL(ent, "failed to get entry");
	ASSERT_INTEQ(ent->next, -1, "collision list has more than 1 entry"); // IMPORTANT check
	ASSERT_STREQ(ent->key, "key", "wrong key value"); // sanity, already confirmed via lookup

	/* ensure value was replaced (2 person in people[]) */
	person_t *p = ent->value;
	ASSERT_STREQ(p->name, "Joan", "wrong entry value");
	ASSERT_INTEQ(p->age, 32, "wrong entry value");

	pcl_htable_free(ht);
	return true;
}

/**$ Put entries with non-string keys */
TESTCASE(htable_nonstrkeys)
{
	pcl_htable_t *ht = pcl_htable(0);

	/* default key_equals is memcmp when key_len is not zero. */
	ht->key_len = sizeof(person_t);

	ASSERT_INTEQ(pcl_htable_put(ht, &people[0], "value 1", true), 0, "wrong return value for put");
	ASSERT_INTEQ(pcl_htable_put(ht, &people[1], "value 2", true), 0, "wrong return value for put");
	ASSERT_INTEQ(pcl_htable_put(ht, &people[2], "value 3", true), 0, "wrong return value for put");

	ASSERT_STREQ(pcl_htable_get(ht, &people[0]), "value 1", "wrong value for value 1");
	ASSERT_STREQ(pcl_htable_get(ht, &people[1]), "value 2", "wrong value for value 2");
	ASSERT_STREQ(pcl_htable_get(ht, &people[2]), "value 3", "wrong value for value 3");

	pcl_htable_free(ht);
	return true;
}
