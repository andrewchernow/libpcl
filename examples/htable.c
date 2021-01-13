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

#include <pcl/defs.h>
#include <pcl/init.h>
#include <pcl/string.h>
#include <pcl/alloc.h>
#include <pcl/htable.h>
#include <pcl/error.h>
#include <pcl/farmhash.h>
#include <pcl/array.h>
#include <stdlib.h>

typedef struct
{
	char *name;
	int age;
} person_t;

/* case-insensitive keys requires lower|upper casing keys before hashing. */
static uintptr_t my_hashcode(const void *key, size_t key_len)
{
	UNUSED(key_len);

	/* use strdup since our key points to person_t.name. We don't want to change that. */
	char *k = pcl_strlower(pcl_strdup(key));

	/* this is what the default hashcode func does */
#ifdef PCL_64BIT
	uint64_t code = pcl_farmhash64(k);
#else
	uint32_t code = pcl_farmhash32(k);
#endif

	pcl_free(k);
	return code;
}

static bool my_key_equals(const void *key1, const void *key2, size_t key_len)
{
	UNUSED(key_len);
	return pcl_stricmp((const char *) key1, (const char *) key2) == 0;
}

static void my_remove_entry(const void *key, void *value)
{
	UNUSED(key);

	person_t *person = (person_t *) value;

	if(person)
	{
		if(person->name)
			pcl_free(person->name);
		pcl_free(person);
	}

	// do not free key, it is the same pointer as person->name in this example
	//pcl_free(key);
}

int main(int argc, char **argv)
{
	UNUSED(argc || argv);
	pcl_init();

	person_t people[] = {
		{"Andrew", 24},
		{"Joan", 32},
		{"Henry", 81},
		{"Marsha", 17},
		{"Linda", 32},
		{"Michael", 67},
		{"Samuel", 53},
		{"Fred", 29},
		{"Eric", 38},
		{"Emily", 63},
		{"Adam", 30},
		{"David", 31},
		{"Carolyn", 78},
		{"Jessica", 18},
	};

	/* set table size to 0 to force at least one rehash. Seetings this to zero will use the
	 * first prime within an internal table: 17. We put 14 entries which would cause a
	 * rehash: 17 * .7 max loadfac = 11.9 so rehash is needed.
	 */
	pcl_htable_t *ht = pcl_htable(0);

	ht->min_loadfac = .3f; // default is .20, example of setting it
	ht->max_loadfac = .7f; // default is .75, example of setting it
	ht->remove_entry = my_remove_entry;
	ht->hashcode = my_hashcode;
	ht->key_equals = my_key_equals;

	for(int i = 0; i < countof(people); i++)
	{
		person_t *d = pcl_malloc(sizeof(person_t));
		
		d->name = pcl_strdup(people[i].name);
		d->age = people[i].age;

		if(pcl_htable_put(ht, d->name, d, true) < 0)
			PANIC(NULL, 0);
	}

	pcl_htable_entry_t *ent = pcl_htable_lookup(ht, "eRiC");

	if(!ent)
		PANIC("didn't find expected key - Eric", 0);

	person_t *p = ent->value;
	printf("Found key=%s, value: name=%s, age=%d\n", (char *) ent->key, p->name, p->age);

	/* vector of keys, char** in this example */
	pcl_array_t *keys = pcl_htable_keys(ht);

	for(int i = 0; i < keys->count; i++)
	{
		char *key = keys->elements[i];

		ent = pcl_htable_lookup(ht, key);

		if(ent == NULL)
			PANIC("can't find entry - %s", key);

		p = (person_t *) ent->value;
		printf("%s is %d years old\n", p->name, p->age);
	}

	pcl_array_free(keys);

	p = pcl_malloc(sizeof(person_t));
	p->name = pcl_strdup("Michael");
	p->age = 65;

	/* replace an existing entry: last argument is false, not-unique */
	pcl_htable_put(ht, p->name, p, false);

	ent = pcl_htable_lookup(ht, "mIchaEl");

	if(!ent)
		PANIC("didn't find expected key", 0);

	p = (person_t *) ent->value;
	printf("%s, age=%d (was 67 before entry replace)\n", p->name, p->age);

	return 0;
}

