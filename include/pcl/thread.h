
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

#ifndef LIBPCL_THREAD_H
#define LIBPCL_THREAD_H

#include <pcl/types.h>
#define _TIMESPEC_DEFINED
#include <pthread.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pcl_thread_start_t)(void *);

PCL_PUBLIC int pcl_thread(pthread_t *t, pcl_thread_start_t routine, void *a);
PCL_PUBLIC uint64_t pcl_thread_id(void);

/* mutex support */
PCL_PUBLIC int pcl_mutex_init(pthread_mutex_t *m);
PCL_PUBLIC int pcl_mutex_lock(pthread_mutex_t *m);
PCL_PUBLIC int pcl_mutex_trylock(pthread_mutex_t *m);
PCL_PUBLIC int pcl_mutex_unlock(pthread_mutex_t *m);
PCL_PUBLIC int pcl_mutex_destroy(pthread_mutex_t *m);

/* TLS support */
PCL_PUBLIC int pcl_tls_alloc(pthread_key_t *key, void (*destructor)(void*));
PCL_PUBLIC int pcl_tls_set(pthread_key_t key, void *value);
PCL_PUBLIC void *pcl_tls_get(pthread_key_t key);
PCL_PUBLIC int pcl_tls_free(pthread_key_t key);

#ifdef __cplusplus
}
#endif
#endif
