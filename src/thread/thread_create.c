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

#include <pcl/thread.h>
#include <pcl/stdlib.h>
#include <pcl/error.h>
#include <pcl/event.h>

typedef struct
{
	void *arg;
	pcl_thread_start_t routine;
} thread_start_t;

static void *
thread_start_wrapper(void *__arg)
{
	thread_start_t *start = __arg;
	void *arg = start->arg;
	void (*routine)(void *) = start->routine;

	pcl_free(start);
	pcl_event_dispatch(PCL_EVENT_THREADINIT, NULL);
	routine(arg);

	return 0;
}

int
pcl_thread_create(pthread_t *t, pcl_thread_start_t routine, void *arg)
{
	pthread_t tbuf;

	if(!routine)
		return SETERR(EINVAL);

	if(!t)
		t = &tbuf;

	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setscope(&attr, PTHREAD_SCOPE_SYSTEM);

	/* create thread arg wrapper and start thread */
	thread_start_t *start = pcl_malloc(sizeof(thread_start_t));
	start->arg = arg;
	start->routine = routine;
	int err = pthread_create(t, &attr, thread_start_wrapper, start);

	pthread_attr_destroy(&attr);

	if(err)
	{
		pcl_free(start);
		return SETOSERR(err);
	}

	return 0;
}
