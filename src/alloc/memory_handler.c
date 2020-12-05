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

#include <pcl/alloc.h>
#include <pcl/atomic.h>
#include <stdio.h>
#include <stdlib.h>

static void
default_memory_handler(const char *type, void *ptr, size_t size, PCL_LOCATION_PARAMS)
{
	fprintf(stderr, "memory error: '%s' failed at %s:%s(%d) - ptr=%p, size=%zd\n",
		type, PCL_LOCATION_VALS, ptr, size);

	exit(1);
}

static pcl_memory_handler_t memory_handler = default_memory_handler;

pcl_memory_handler_t
pcl_set_memory_handler(pcl_memory_handler_t handler)
{
	/* swap handler atomically */
	return (pcl_memory_handler_t) pcl_atomic_exchange(
		(pcl_atomic_t *) &memory_handler, (pcl_atomic_t) handler);
}

void
pcl_memory_error(const char *type, void *ptr, size_t siz, PCL_LOCATION_PARAMS)
{
	pcl_memory_handler_t h;

	/* fetch handler atomically, can't just access it and be thread-safe */
	h = (pcl_memory_handler_t) pcl_atomic_fetch((pcl_atomic_t *) &memory_handler);

	if(h)
		h(type, ptr, siz, PCL_LOCATION_VALS);
}

