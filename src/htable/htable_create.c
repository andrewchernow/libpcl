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
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/farmhash.h>
#include <string.h>

static bool
default_key_equals(const void *a, const void *b, size_t key_len, void *u)
{
	UNUSED(u);
	return key_len ? !memcmp(a, b, key_len) : !strcmp((const char*) a, (const char*) b);
}

/* Default hashfunc is Google's farmhash: https://github.com/google/farmhash */
static uintptr_t
default_hashcode(const void *key, size_t key_len, void *userp)
{
	UNUSED(userp);

	if(!key_len)
		key_len = strlen((const char *) key);

#ifdef PCL_64BIT
	return (uintptr_t) pcl_farmhash64_seed(key, key_len, 0);
#else
	return (uintptr_t) pcl_farmhash32_seed(key, key_len, 0);
#endif
}

pcl_htable_t *
pcl_htable_create(int capacity)
{
	capacity = ipcl_htable_capacity(max(capacity, 0));

	if(capacity < 0)
		return R_TRC(NULL);

	pcl_htable_t *ht = pcl_zalloc(sizeof(pcl_htable_t));

	ht->key_len = 0;
	ht->capacity = capacity;
	ht->min_loadfac = MIN_LOADFAC;
	ht->max_loadfac = MAX_LOADFAC;
	ht->key_equals = default_key_equals;
	ht->hashcode = default_hashcode;
	ht->entries = pcl_zalloc(ht->capacity * sizeof(void *));

	return ht;
}
