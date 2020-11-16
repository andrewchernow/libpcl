
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

#ifndef LIBPCL_HTABLE_H
#define LIBPCL_HTABLE_H

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Hash table entry object. The overhead of each entry is 16 bytes on 32-bit machines and
 * 32 bytes on 64-bit machines.
 */
typedef struct tag_pcl_htable_entry
{
	/** leave me alone (collision list) */
	struct tag_pcl_htable_entry *next;

	/** hash code of entry key, 4 bytes on 32-bit and 8 bytes on 64-bit */
	uintptr_t code;

	/** entry key */
	const char *key;

	/** entry value */
	void *value;
} pcl_htable_entry_t;

struct tag_pcl_htable
{
	/* READONLY SECTION */

	/** count of entries */
	int count;

	/** current table capacity */
	int capacity;

	/** table of entries, each bucket is a linked list of collisions */
	pcl_htable_entry_t **entries;

	/* READ/WRITE SECTION */

	/** The minimun load factor of the table. When the number of entries in the table
	 * falls below the product of this value and current capacity, the table is shrunk to
	 * half its capacity rounded up to the nearest prime. pcl_htable_create defaults this value
	 * to 0.20. If a different value is desired, set this directly after create.
	 */
	float min_loadfac;

	/** The maximum load factor of the table. When the number of entries in the table
 	 * exceeds the product of this value and current capacity, the table is grown to
 	 * double its capacity rounded up to the nearest prime. pcl_htable_create defaults this value
 	 * to 0.75. If a different value is desired, set this directly after create.
 	 */
	float max_loadfac;

	/** User pointer passed to callbacks */
	void *userp;

	/** Called whenever an entry is being removed from the table. By default, this is set to
	 * NULL, which means the "entry" object itself is freed but key and value are not. There
	 * are cases where this applicable. However, it is more common that key and value need to
	 * be freed by the application.
	 */
	void (*remove_entry)(const char *key, void *value, void *userp);

	/** Determine if two keys are equal. The default comparator uses strcmp. For case-insensative
	 * compares, provide a key_equals callback that uses pcl_stricmp.
	 * @return true if key1 and key2 are equal
	 */
	bool (*key_equals)(const char *key1, const char *key2, void *userp);

	/** Compute a 32 or 64-bit hash code for the given key. Google's farmhash is used as the
	 * default hashfunc for both 32 and 64-bit architectures. It is highly recommened to NOT
	 * use a 64-bit code on 32-bit machines.
	 */
	uintptr_t (*hashcode)(const char *key, void *userp);
};

/** Creates a new hash table object. All hash table callbacks are set to NULL, using the
 * default implementations as documented. After creating a hash table, you can assign callbacks
 * and a user pointer as needed.
 *
 * @param capacity initial capacity of hash table. This is rounded up to the nearest prime.
 * @return hash table pointer or NULL on error.
 */
PCL_EXPORT pcl_htable_t *pcl_htable_create(int capacity);

/** Lookup and return an entry.
 * @param ht
 * @param key pointer to the key to lookup
 * @return pointer to the key's entry or NULL if not found or an error occurs. If not found,
 * the last PCL error (pcl_errno) is set to PCL_ENOTFOUND.
 */
PCL_EXPORT pcl_htable_entry_t *pcl_htable_lookup(pcl_htable_t *ht, const char *key);

/** Lookup an entry and return its value.
 * @param ht
 * @param key
 * @return value pointer or NULL if not found. values can be NULL. To detect an error verse a
 * NULL value, check pcl_errno: if(value == NULL && pcl_errno == PCL_EOKAY) success;
 */
PCL_INLINE void *pcl_htable_get(pcl_htable_t *ht, const char *key);

/** Puts a key/value pair into the table.
 * @param ht
 * @param key pointer to the string key name. This is shallow assignment.
 * @param value pointer to the entry's value. This is shallow assignment.
 * @param unique indicates if the put operation should enforce unique keys. When true, if
 * a key exists the operation fails. When false, a matching entry's key and value are replaced
 * with the given key and value; remove_entry called with old key and value.
 * @return 0 on success and -1 on error. For unique puts, pcl_errno is set to PCL_EEXISTS.
 */
PCL_EXPORT int pcl_htable_put(pcl_htable_t *ht, const char *key, void *value, bool unique);

/** Remove an entry from the table. If the key does not exist, this call is silently ignored.
 * @param ht
 * @param key pointer to the entry's key to remove
 */
PCL_EXPORT void pcl_htable_remove(pcl_htable_t *ht, const char *key);

/** Get hash table keys as a vector. Note, each element is a 'char**' so ensure to use
 * pcl_vector_getptr or _getptr_s to retreive the keys from the vector.
 * @param ht
 * @return vector on success and NULL on error. An empty table produces an empty vector. This
 * vector must be freed by caller.
 */
PCL_EXPORT pcl_vector_t *pcl_htable_keys(pcl_htable_t *ht);

/** Clear all entries from the table.
 * @param ht
 * @param shrink when true, the table will be shrunk down to the smallest table size; 17.
 */
PCL_EXPORT void pcl_htable_clear(pcl_htable_t *ht, bool shrink);

/* If remove_entry callback was supplied, it will be called for each entry
 * currently within the hash table. Always returns NULL, which makes a
 * free and set NULL operation one statement: `ht = pcl_htable_free(ht);`
 */
PCL_EXPORT void *pcl_htable_free(pcl_htable_t *ht);

#ifdef __cplusplus
}
#endif
#endif

