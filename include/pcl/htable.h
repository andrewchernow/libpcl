
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

#ifndef LIBPCL_HTABLE_H
#define LIBPCL_HTABLE_H

/** @defgroup htable Hash Table
 * This defines a hash table object and associated management functions. This is implemented as
 * an array of linked lists, with the ability to set a minimum and maximum load factor.
 * There are three callbacks to customize a hash table: pcl_htable_t.remove_entry,
 * pcl_htable_t.hashcode and pcl_htable_t.key_equals. All three can be assigned after table
 * creation.
 *
 * ### Rehashing
 * Rehashing is implemented by allocating a new table (buckets of entry lists)
 * that is either double the current size rounded up to the nearest prime or half the current
 * size rounded up to the nearest prime. In either case, the entries are shallow copied to
 * the new table after computing their bucket indexes. When the copy is complete, the old table
 * is freed. No hash codes are recomputed, since each entry caches its own hash code.
 *
 * ### Table Size Limitations
 * The table can grow to ~50 million on 32-bit machines and ~1.6 billion on 64-bit machines. This
 * table expands and contracts based on a max and min load factor -- 0.75 and 0.20 respectively.
 *
 * ### String and Binary Keys
 * The PCL hash table supports binary keys, which are fixed-length and normally do not change for
 * the life of the hash table. After creating a hash table, one can specify the key length in bytes
 * or provide zero (the default) for variable-length keys (strings). For example, when using a UUID
 * string as a key, each key would require 36 bytes. However, only 16 bytes per key if binary. In
 * this case, you would set the key_len to 16. The default \c key_equals will use \c memcmp when
 * \a key_len is not zero and \c strcmp otherwise.
 *
 * #### UUID Key Example
 * @code
 * #include <pcl/htable.h>
 * #include <pcl/init.h>
 * #include <pcl/string.h>
 * #include "app/user.h"
 *
 * static unsigned char *uuidtokey(unsigned char *out, const char *uuid)
 * {
 *   char *hex = pcl_strreplace(uuid, "-", "");
 *   pcl_hex_decode(out, 16, hex);
 *   pcl_free(hex);
 *   return out;
 * }
 *
 * static void remove_entry(const void *key, void *value, void *userp)
 * {
 *   UNUSED(userp);
 *   pcl_free(key);
 *   app_user_free((app_user_t *) value);
 * }
 *
 * int main(void)
 * {
 *   pcl_init();
 *
 *   int num;
 *   app_user_t **users = app_user_fetchall(&num);
 *
 *   pcl_htable_t *ht = pcl_htable_create(num);
 *
 *   // let htable know key length
 *   ht->key_len = 16;
 *   ht->remove_entry = remove_entry;
 *
 *   unsigned char uuid[16];
 *
 *   for(int i = 0; i < num; i++)
 *   {
 *     void *key = memcmp(pcl_malloc(16), uuidtokey(uuid, users[i]->uuid), 16);
 *     pcl_htable_put(ht, key, users[i], true);
 *   }
 *
 *   const char *john = "37462d98-371b-11eb-adc1-0242ac120002";
 *   pcl_user_t *user = pcl_htable_get(ht, uuidtokey(uuid, john));
 *
 *   printf("%s\n", user->surname);
 *
 *   pcl_htable_free(ht);
 *
 *   return 0;
 * }
 *
 * @endcode
 *
 * One step further, is taking a 32 or 64-bit sequence value, like a \c user_id, and storing its
 * value as the key pointer address: `key = (const void *)(uintptr_t) user_id`. This would require
 * a custom key_equals (where key1 == key2 could be the implementation) and a custom
 * hashcode callback (where return (uintptr_t) key; could be the implementation). This method
 * is very efficient both in memory and performance, but only works if the numeric bit length is
 * \c <= the number of bits used by pointers. An example of this is below.
 *
 * #### UserId Key Example
 * @code
 * #include <pcl/htable.h>
 * #include <pcl/init.h>
 * #include "app/user.h"
 *
 * #define IDTOKEY(id) (const void *) ((uintptr_t) (id))
 *
 * static bool key_equals(const void *a, const void *b, size_t key_len, void *userp)
 * {
 *   UNUSED(key_len || userp);
 *   return a == b; // key addresses are user_ids, compare them directly.
 * }
 *
 * static uintptr_t hashcode(const void *key, size_t key_len, void *userp)
 * {
 *   UNUSED(key_len || userp);
 *   return (uintptr_t) key; // no need to hash keys, user_id is unique
 * }
 *
 * static void remove_entry(const void *key, void *value, void *userp)
 * {
 *   UNUSED(key || userp);
 *   app_user_free((app_user_t *) value); // key doesn't need to be freed
 * }
 *
 * int main(void)
 * {
 *   pcl_init();
 *
 *   int num_users;
 *   app_user_t **users = app_user_fetchall(&num_users);
 *
 *   // a table mapping a user_id => user.
 *   pcl_htable_t *ht = pcl_htable_create(num_users);
 *
 *   // configuring a pcl_htable_t
 *   ht->remove_entry = remove_entry;
 *   ht->hashcode = hashcode;
 *   ht->key_equals = key_equals;
 *
 *   // ignored since example sets hashcode and key_equals. Set if your callbacks need it.
 *   //ht->key_len = 0;
 *
 *   //ht->min_loadfac = 0.13f;
 *   //ht->max_loadfac = 0.85f;
 *   //ht->userp = used_by_callbacks;
 *
 *   // set key address to user_id
 *   for(int i = 0; i < num_users; i++)
 *     pcl_htable_put(ht, IDTOKEY(users[i]->id), users[i], true);
 *
 *   pcl_free(users);
 *
 *   app_user_t *user = pcl_htable_get(ht, IDTOKEY(1982734));
 *
 *   printf("%s\n", user->name);
 *
 *   pcl_htable_free(ht); // calls remove_entry foreach entry in table
 *
 *   return 0;
 * }
 * @endcode
 * @{
 */
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
	const void *key;

	/** entry value */
	void *value;
} pcl_htable_entry_t;

struct tag_pcl_htable
{
	/* READONLY SECTION */

	/** count of entries
	 * @warning treat this as immutable
	 */
	int count;

	/** current table capacity
	 * @warning treat this as immutable
	 */
	int capacity;

	/** table of entries, each bucket is a linked list of collisions
	 * @warning treat this as immutable
	 */
	pcl_htable_entry_t **entries;

	/* READ/WRITE SECTION */

	/** Key length in bytes. The default is zero, which means variable-length (strings). When
 	 * implementing \a key_equals and \a hashcode callbacks, this value is not used by the table.
 	 * When using the default \a key_equals, \c strcmp is used when this is zero and \c memcmp
 	 * when non-zero. When using the default \c hashcode, \c strlen is used when this is zero to
 	 * compute the \a key_len otherwise this value is used directly.
 	 *
 	 * The table should be empty when setting this value: after ::pcl_htable_create
 	 * or ::pcl_htable_clear.
 	 */
	size_t key_len;

	/** The minimun load factor of the table. When the number of entries in the table
	 * falls below the product of this value and current capacity, the table is shrunk to
	 * half its capacity rounded up to the nearest prime. The default value is \c 0.20f.
	 *
 	 * The table should be empty when setting this value: after ::pcl_htable_create
 	 * or ::pcl_htable_clear.
	 */
	float min_loadfac;

	/** The maximum load factor of the table. When the number of entries in the table
 	 * exceeds the product of this value and current capacity, the table is grown to
 	 * double its capacity rounded up to the nearest prime. The default is \c 0.75f.
 	 *
 	 * The table should be empty when setting this value: after ::pcl_htable_create
 	 * or ::pcl_htable_clear.
 	 */
	float max_loadfac;

	/** User pointer passed to callbacks */
	void *userp;

	/** Called when an entry is being removed from the table. There is no default version, this
	 * is set to \c NULL on the hash table. When \c NULL, only the entry object itself is freed.
	 * In most cases, an application will want to set this to free keys and/or values.
	 * @param key pointer to the key beign removed
	 * @param value pointer to the value
	 * @param userp pointer to optional user data. This can be \c NULL.
	 */
	void (*remove_entry)(const void *key, void *value, void *userp);

	/** Determine if two keys are equal. The default comparator uses \c strcmp when \a key_len
	 * is zero and \c memcmp otherwise.
	 * @param key1 pointer to a key
	 * @param key1 pointer to a key to compare against \a key1
	 * @param key_len key length in bytes
	 * @param userp pointer to optional user data. This can be \c NULL.
	 * @return true if \a key1 and \a key2 are equal
	 */
	bool (*key_equals)(const void *key1, const void *key2, size_t key_len, void *userp);

	/** Compute a hash code for the given key. Google's farmhash is used as the default hashfunc
	 * for both 32 and 64-bit architectures. On 32-bit machines, a 32-bit code is computed and
	 * on 64-bit machines, a 64-bit hash code.
	 *
	 * Hash code values use the \c uintptr_t data type. This means the number of bits allowed
	 * depends on the number of bits within a \c uintptr_t.
	 * @param key pointer to the key to hash
	 * @param key_len key length in bytes
	 * @param userp pointer to optional user data. This can be \c NULL.
	 * @return hash code no more than \c unitptr_t bytes
	 */
	uintptr_t (*hashcode)(const void *key, size_t key_len, void *userp);
};

/** Creates a new hash table object. All hash table callbacks are set to the default
 * implementations as documented. After creating a hash table, you can assign callbacks.
 * @param capacity initial capacity of hash table. This is rounded up to the nearest prime.
 * @return hash table pointer or NULL on error.
 */
PCL_EXPORT pcl_htable_t *pcl_htable_create(int capacity);

/** Lookup and return an entry.
 * @param ht pointer to hash table object
 * @param key pointer to the key to lookup
 * @return pointer to the key's entry or NULL if not found or an error occurs. If not found,
 * the last PCL error (pcl_errno) is set to PCL_ENOTFOUND.
 */
PCL_EXPORT pcl_htable_entry_t *pcl_htable_lookup(pcl_htable_t *ht, const void *key);

/** Lookup an entry and return its value.
 * @param ht pointer to hash table object
 * @param key pointer to a key
 * @return value pointer or NULL if not found. values can be NULL. To detect an error verse a
 * NULL value, check pcl_errno: if(value == NULL && pcl_errno == PCL_EOKAY) success;
 */
PCL_EXPORT void *pcl_htable_get(pcl_htable_t *ht, const void *key);

/** Puts a key/value pair into the table.
 * @param ht pointer to hash table object
 * @param key pointer to the key. This is shallow assignment.
 * @param value pointer to the entry's value. This is shallow assignment.
 * @param unique indicates if the put operation should enforce unique keys. When true, if
 * a key exists the operation fails. When false, a matching entry's key and value are replaced
 * with the given key and value; remove_entry called with old key and value.
 * @return 0 on success and -1 on error. For unique puts, pcl_errno is set to PCL_EEXISTS.
 */
PCL_EXPORT int pcl_htable_put(pcl_htable_t *ht, const void *key, void *value, bool unique);

/** Remove an entry from the table. If the key does not exist, this call is silently ignored.
 * @param ht pointer to hash table object
 * @param key pointer to the entry's key to remove
 */
PCL_EXPORT void pcl_htable_remove(pcl_htable_t *ht, const void *key);

/** Get hash table keys as an array.
 * @param ht pointer to hash table object
 * @return array on success and \c NULL on error. An empty table produces an empty array. This
 * array must be freed by caller.
 */
PCL_EXPORT pcl_array_t *pcl_htable_keys(pcl_htable_t *ht);

/** Clear all entries from the table.
 * @param ht pointer to hash table object
 * @param shrink when true, the table will be shrunk down to the smallest table size; 17.
 */
PCL_EXPORT void pcl_htable_clear(pcl_htable_t *ht, bool shrink);

/** Release all resources used by the given hash table.
 * If a remove_entry callback is set on the given hash table, it will be called for each entry.
 * @param ht pointer to hash table object
 * @return always returns NULL, which is useful as an assignment and free in a single line:
 * `my_obj->ht = pcl_htable_free(my_obj->ht)`
 */
PCL_EXPORT void *pcl_htable_free(pcl_htable_t *ht);

#ifdef __cplusplus
}
#endif

/** @} */
#endif

