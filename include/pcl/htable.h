
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

#ifndef LIBPCL_HTABLE_H
#define LIBPCL_HTABLE_H

/** @defgroup htable Hash Table
 * This defines a hash table object and associated management functions. This implementation
 * will preserve insertion order: like Java's LinkedHashMap but without the doubly-linked list.
 *
 * ### Implementation
 * Whenever an entry is put, it is added sequentially to an array of entry
 * objects. To handle collisions, the entry objects are a linked list using an integer value
 * that represents the index of the next collision within the entry array. The key is hashed
 * using Google's FarmHash and bitwise AND'd with the current table size minus 1 to get the
 * hashed index value. The hashed index value is an index to a second array (entry_lookup) of
 * entry array indexes. Thus, looking up a hashed index within the \a entry_lookup array, will
 * give you the entry array index, which is the beginning of the entry collision list. This is how
 * the PCL hash table can preserve insertion order: one array using hash index to find the index
 * into the sequential array of entries.
 *
 * @note The entry_lookup and entry array idea came directly from PHP's new hash table
 * implementation released in 7.0. Just like PHP, PCL allocates the entry and entry_lookup arrays
 * as one allocation: both with \a capacity elements. When an entry is removed, the entry array
 * marks it as deleted but does not adjust the array to close the gap. An entry is marked as
 * deleted by setting its \a key to \c NULL, which PCL treats as an invalid key. On rehash, the
 * entry array is defragmented.
 *
 * ### Default Hash Function
 * Unlike PHP's implementation, PCL does not use the DJBX33A hash as its default, which stands
 * for “Daniel J. Bernstein, Times 33 with Addition”. Google's farmhash proved to have far less
 * collisions (on string data since DJBX33A is for string keys). In one test of 470,000
 * english words, Google's FarmHash had no collisions while DJBX33A had 320. Performance wise,
 * they are about the same. Another benifit of Google's FarmHash is that it is not just designed
 * for string keys, which PCL requires to allow binary and string keys. It is also prefered
 * to provide a single generalized hash function for both binary and string keys out of the box.
 *
 * ### Customizing
 * This implementation has the ability to set a minimum and maximum load factor. You can also
 * set the \a key_len if not using string keys. If key_len is 0 (strings), then the default
 * key_equals function uses strcmp. If key_len is not 0, memcmp is used. There are three callbacks
 * to customize a hash table: pcl_htable_t.remove_entry, pcl_htable_t.hashcode and
 * pcl_htable_t.key_equals. All of these customizations can be set after table creation.
 *
 * ### Rehashing
 * Rehashing is implemented by allocating a new entry and entry_lookup array (single allocation)
 * that is either double the current size or half the current size (uses powers of 2). When the
 * copy is complete, the old entry and entry_lookup arrays are freed. No hash codes are recomputed,
 * since each entry caches its own hash code. The entry array is defragmented, meaning all deleted
 * entries are removed; \a count_used will always be the same as \a count.
 *
 * ### Table Size Limitations
 * The table can grow to 33,554,432 on 32-bit machines and 1,073,741,824 on 64-bit machines. This
 * table expands and contracts based on a max and min load factor -- 0.75 and 0.20 respectively.
 * Table capacity is always powers of two.
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
 * static void remove_entry(const void *key, void *value)
 * {
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
 *   pcl_htable_t *ht = pcl_htable(num);
 *
 *   // let htable know key length, only used by default key_equals and hashcode callbacks
 *   ht->key_len = 16;
 *   ht->remove_entry = remove_entry;
 *
 *   unsigned char uuid[16];
 *
 *   for(int i = 0; i < num; i++)
 *   {
 *     void *key = pcl_malloc(16);
 *     memcpy(key, uuidtokey(uuid, users[i]->uuid), 16);
 *     pcl_htable_put(ht, key, users[i], true);
 *   }
 *
 *   // elements will be freed by hash table's remove_entry
 *   pcl_free(users);
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
 * \c <= the number of bits used by pointers. Also, user_id of 0 is invalid, since keys cannot
 * be \c NULL, but it is uncommon to start sequence values like user_ids with 0. An example of
 * this is below.
 *
 * #### UserId Key Example
 * @code
 * #include <pcl/htable.h>
 * #include <pcl/init.h>
 * #include "app/user.h"
 *
 * #define IDTOKEY(id) (const void *) ((uintptr_t) (id))
 *
 * static bool key_equals(const void *a, const void *b, size_t key_len)
 * {
 *   UNUSED(key_len);
 *   return a == b; // key addresses are user_ids, compare them directly.
 * }
 *
 * static uintptr_t hashcode(const void *key, size_t key_len)
 * {
 *   UNUSED(key_len);
 *   return (uintptr_t) key; // no need to hash keys, user_id is unique
 * }
 *
 * static void remove_entry(const void *key, void *value)
 * {
 *   UNUSED(key);
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
 *   pcl_htable_t *ht = pcl_htable(num_users);
 *
 *   // configuring a pcl_htable_t
 *   ht->remove_entry = remove_entry;
 *   ht->hashcode = hashcode;
 *   ht->key_equals = key_equals;
 *
 *   // ignored since example sets hashcode and key_equals. Set if your callbacks need it.
 *   //ht->key_len = sizeof(int64_t);
 *
 *   // set min and max load factor
 *   //ht->min_loadfac = 0.13f;
 *   //ht->max_loadfac = 0.85f;
 *
 *   // set key address to user_id
 *   for(int i = 0; i < num_users; i++)
 *     pcl_htable_put(ht, IDTOKEY(users[i]->id), users[i], true);
 *
 *   pcl_free(users); // elements freed by remove_entry
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
 * 32 bytes on 64-bit machines. On 64-bit machines, this is really 28 bytes but will be
 * padded to 32.
 */
typedef struct
{
	/** leave me alone (collision list) */
	int next;

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

	/** count of entries. differs from \a count_used because this doesn't include deleted entries.
	 * @warning treat this as immutable
	 */
	int count;

	/** count of used entries (active + deleted). A deleted entry has a \c NULL key and
	 * should always be skipped.
	 * @warning treat this as immutable
	 */
	int count_used;

	/** current table capacity
	 * @warning treat this as immutable
	 */
	int capacity;

	/** mask used to find buckets, table `capacity - 1`
	 * @warning treat this as immutable
	 */
	int table_mask;

	/** A sequential array of entries as they are inserted. This not allows for preserving
	 * insertion order, by avoids allocating each individual entry. This always contains
	 * \a capacity available entries. The next insert index is bound to \a count_used, not
	 * \a count. \a count_used is always incremented, unlike \a count. It is reset to 0
	 * by ::pcl_htable_clear and reset to \a count during an internal rehash.
	 * @note each entry element is part of a collision list by using its \a next
	 * member to indicate the index of the next entry in the list. A \a next value of -1 indicates
	 * the end of the list.
	 * @note allocated in same block of memeory as \a entry_lookup
	 */
	pcl_htable_entry_t *entries;

	/** An entry lookup array where each element is an \a entries index value. When given a key,
	 * the key's hashcode is bitwise AND'd with the table's \a capacity minus 1, which produces the
	 * index to this array; termed the hashed index or \c hashidx. Elements in this array are set
	 * to -1 to indicate they are not in use. This always contains \a capacity indexes.
	 * @note allocated in same block of memeory as \a entries
	 */
	int *entry_lookup;

	/* READ/WRITE SECTION */

	/** Key length in bytes. The default is zero, which means variable-length (strings). This value
	 * is only used by the default \a key_equals and \a hashcode callbacks. If a custom version of
	 * both callbacks are set, this can remain unset unless the custom implementation requires
	 * this value.
 	 *
 	 * The table should be empty when setting this value: after ::pcl_htable
 	 * or ::pcl_htable_clear.
 	 */
	size_t key_len;

	/** The minimun load factor of the table. When the number of entries in the table
	 * falls below the product of this value and current capacity, the table is shrunk to
	 * half its capacity (always a power of 2). The default value is \c 0.20f.
	 *
 	 * The table should be empty when setting this value: after ::pcl_htable
 	 * or ::pcl_htable_clear.
	 */
	float min_loadfac;

	/** The maximum load factor of the table. When the number of entries in the table
 	 * exceeds the product of this value and current capacity, the table is grown to
 	 * double its capacity (always a power of 2). The default is \c 0.75f.
 	 *
 	 * The table should be empty when setting this value: after ::pcl_htable
 	 * or ::pcl_htable_clear.
 	 */
	float max_loadfac;

	/** Called when an entry is being removed from the table. There is no default version, this
	 * is set to \c NULL on the hash table. When \c NULL, the \a key and \a value are not freed
	 * which might be the desired behavior. In most cases, an application will want to set this to
	 * free keys and/or values. In either case, the entry itself is marked as deleted (\c NULL key).
	 * @param key pointer to the key beign removed
	 * @param value pointer to the value
	 */
	void (*remove_entry)(const void *key, void *value);

	/** Determine if two keys are equal. The default comparator uses \c strcmp when \a key_len
	 * is zero and \c memcmp otherwise. The \a key_len passed is the pcl_htable_t.key_len member.
	 * @param key1 pointer to a key
	 * @param key1 pointer to a key to compare against \a key1
	 * @param key_len key length in bytes, expected to be the length for both \a key1 and \a key2
	 * @return true if \a key1 and \a key2 are equal
	 */
	bool (*key_equals)(const void *key1, const void *key2, size_t key_len);

	/** Compute a hash code for the given key. Google's farmhash is used as the default hashfunc
	 * for both 32 and 64-bit architectures. On 32-bit machines, a 32-bit code is computed and
	 * on 64-bit machines, a 64-bit hash code.
	 *
	 * For the default hashcode implementation, if \a key_len is zero, it is assumed that \a key is
	 * a string and \c strlen is used to get the \a key_len. The \a key_len passed is the
	 * pcl_htable_t.key_len member.
	 *
	 * This is only executed once for each entry put into the table. The computed hash code is
	 * cached within the entry object. This is also executed, on the \a key argument, each time
	 * either ::pcl_htable_get, ::pcl_htable_lookup or ::pcl_htable_remove are called.
	 *
	 * Hash code values use the \c uintptr_t data type. This means the number of bits allowed
	 * depends on the number of bits within a \c uintptr_t.
	 * @param key pointer to the key to hash
	 * @param key_len key length in bytes
	 * @return hash code no more than \c unitptr_t bytes
	 */
	uintptr_t (*hashcode)(const void *key, size_t key_len);
};

/** Creates a new hash table object. All hash table callbacks are set to the default
 * implementations as documented. After creating a hash table, you can assign callbacks.
 * @param capacity initial capacity of hash table. If this is not a power of 2, it is rounded
 * up to the next power of 2. A value less than 8, will set capacity to 8 (smallest table size).
 * @return hash table pointer or NULL on error.
 */
PCL_PUBLIC pcl_htable_t *pcl_htable(int capacity);

/** Lookup and return an entry.
 * @param ht pointer to hash table object
 * @param key pointer to the key to lookup
 * @return pointer to the key's entry or NULL if not found or an error occurs. If not found,
 * the last PCL error (pcl_errno) is set to PCL_ENOTFOUND.
 */
PCL_PUBLIC pcl_htable_entry_t *pcl_htable_lookup(const pcl_htable_t *ht, const void *key);

/** Lookup an entry and return its value. This is a convenience function that wraps
 * ::pcl_htable_lookup.
 * @param ht pointer to hash table object
 * @param key pointer to a key
 * @return value pointer or NULL if not found. values can be NULL. To detect an error verse a
 * NULL value, check pcl_errno: if(value == NULL && pcl_errno == PCL_EOKAY) success; Or, use
 * ::pcl_htable_lookup that never returns \c NULL on success.
 */
PCL_PUBLIC void *pcl_htable_get(const pcl_htable_t *ht, const void *key);

/** Puts a key/value pair into the table.
 * @param ht pointer to hash table object
 * @param key pointer to the key. This is shallow assignment.
 * @param value pointer to the entry's value. This is shallow assignment.
 * @param unique indicates if the put operation should enforce unique keys. When true, if
 * a key exists the operation fails. When false, a matching entry's key and value are replaced
 * with the given key and value; remove_entry called with old key and value.
 * @return 0 on success and -1 on error. For unique puts, pcl_errno is set to PCL_EEXISTS.
 */
PCL_PUBLIC int pcl_htable_put(pcl_htable_t *ht, const void *key, void *value, bool unique);

/** Remove an entry from the table. If the key does not exist, this call is silently ignored.
 * @param ht pointer to hash table object
 * @param key pointer to the entry's key to remove
 * @return new count of entries or -1 on error
 */
PCL_PUBLIC int pcl_htable_remove(pcl_htable_t *ht, const void *key);

/** Get hash table keys as an array.
 * @param ht pointer to hash table object
 * @return array on success and \c NULL on error. An empty table produces an empty array. This
 * array must be freed by caller.
 */
PCL_PUBLIC pcl_array_t *pcl_htable_keys(const pcl_htable_t *ht);

/** Iterate through a table's entries. This automatically skips deleted entries, which means
 * the given \a index pointer can skip indexes during the iteration: like given 1 on input
 * could produce 5 on output.
 * @code
 * // typical usage
 * int index = 0;
 * pcl_htable_entry_t *ent;
 *
 * // the iteration is always insertion order. the index can skip index values,
 * // which means it skipped one or more deleted entries.
 * while((ent = pcl_htable_iter(ht, &index)))
 *   printf("entries[%d] %s\n", index - 1, ent->key);
 *
 * // can also use this to retrieve the N-th entry
 * int index = 9; // logical 10-th entry inserted, physical depends on num deleted
 * pcl_htable_entry_t *ent = pcl_htable_iter(ht, &index);
 * @endcode
 * @param ht pointer to a hash table
 * @param index pointer to an integer that on input, represents the N-th entry to retrieve. On
 * output, this represents the next index value. This cannot be less than zero or greater than
 * \a count_used. This is typically set to zero to iterate through all entries.
 * @return pointer to the next entry or \c NULL when complete.
 */
PCL_PUBLIC pcl_htable_entry_t *pcl_htable_iter(pcl_htable_t *ht, int *index);

/** Clear all entries from the table.
 * @param ht pointer to hash table object
 * @param shrink when true, the table will be shrunk down to the smallest table size; 17.
 */
PCL_PUBLIC void pcl_htable_clear(pcl_htable_t *ht, bool shrink);

/** Release all resources used by the given hash table.
 * If a remove_entry callback is set on the given hash table, it will be called for each entry.
 * @param ht pointer to hash table object
 * @return always returns NULL, which is useful as an assignment and free in a single line:
 * `my_obj->ht = pcl_htable_free(my_obj->ht)`
 */
PCL_PUBLIC void *pcl_htable_free(pcl_htable_t *ht);

#ifdef __cplusplus
}
#endif

/** @} */
#endif

