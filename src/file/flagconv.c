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

#include "_file.h"
#include <fcntl.h>

int
ipcl_oflags_to_fflags(int oflags)
{
	int flags = 0;

	if(oflags & (PCL_O_RDONLY | PCL_O_RDWR))
		flags |= PCL_FF_READ;

	if(oflags & (PCL_O_WRONLY | PCL_O_RDWR))
		flags |= PCL_FF_WRITE;

	if(oflags & PCL_O_APPEND)
		flags |= PCL_FF_APPEND;

	if(oflags & PCL_O_NONBLOCK)
		flags |= PCL_FF_NONBLOCK;

	return flags;
}

int
ipcl_sysoflags(uint32_t pcl_oflags)
{
	int oflags = 0;

#ifdef PCL_WINDOWS
	oflags |= _O_BINARY;
#endif

	if(pcl_oflags & PCL_O_RDONLY)
	{
		#ifdef _O_RDONLY
		oflags |= _O_RDONLY;
		#elif defined(O_RDONLY)
		oflags |= O_RDONLY;
		#endif
	}

	if(pcl_oflags & PCL_O_WRONLY)
	{
		#ifdef _O_WRONLY
		oflags |= _O_WRONLY;
		#elif defined(O_WRONLY)
		oflags |= O_WRONLY;
		#endif
	}

	if(pcl_oflags & PCL_O_RDWR)
	{
		#ifdef _O_RDWR
		oflags |= _O_RDWR;
		#elif defined(O_RDWR)
		oflags |= O_RDWR;
		#endif
	}

	if(pcl_oflags & PCL_O_CREAT)
	{
		#ifdef _O_CREAT
		oflags |= _O_CREAT;
		#elif defined(O_CREAT)
		oflags |= O_CREAT;
		#endif
	}

	if(pcl_oflags & PCL_O_APPEND)
	{
		#ifdef _O_APPEND
		oflags |= _O_APPEND;
		#elif defined(O_APPEND)
		oflags |= O_APPEND;
		#endif
	}

	if(pcl_oflags & PCL_O_TRUNC)
	{
		#ifdef _O_TRUNC
		oflags |= _O_TRUNC;
		#elif defined(O_TRUNC)
		oflags |= O_TRUNC;
		#endif
	}

	if(pcl_oflags & PCL_O_EXCL)
	{
		#ifdef _O_EXCL
		oflags |= _O_EXCL;
		#elif defined(O_EXCL)
		oflags |= O_EXCL;
		#endif
	}

	if(pcl_oflags & PCL_O_NONBLOCK)
	{
		#ifdef O_NONBLOCK
		oflags |= O_NONBLOCK;
		#elif defined(O_NDELAY)
		oflags |= O_NDELAY;
		#endif
	}

	if(pcl_oflags & PCL_O_CLOEXEC)
	{
		#ifdef _O_NOINHERIT
		oflags |= _O_NOINHERIT;
		#elif defined(O_CLOEXEC)
		oflags |= O_CLOEXEC;
		#endif
	}

	/* only linux supports this right now */
	if(pcl_oflags & PCL_O_NOATIME)
	{
		#ifdef O_NOATIME
		oflags |= O_NOATIME;
		#endif
	}

	if(pcl_oflags & PCL_O_DIRECTORY)
	{
		#ifdef O_DIRECTORY
		oflags |= O_DIRECTORY;
		#endif
	}

	/* Manually implemented by PCL when not available: like windows */
	if(pcl_oflags & PCL_O_NOFOLLOW)
	{
		#ifdef O_NOFOLLOW
		oflags |= O_NOFOLLOW;
		#endif
	}

	if(pcl_oflags & PCL_O_SYNC)
	{
		#ifdef O_SYNC
		oflags |= O_SYNC;
		#else
		#ifdef O_DSYNC
				oflags |= O_DSYNC;
			#endif
			#ifdef O_FSYNC
				oflags |= O_FSYNC;
			#endif
		#endif
	}

	if(pcl_oflags & PCL_O_SHLOCK)
	{
		#ifdef O_SHLOCK
		oflags |= O_SHLOCK;
		#endif
	}

	if(pcl_oflags & PCL_O_EXLOCK)
	{
		#ifdef O_EXLOCK
		oflags |= O_EXLOCK;
		#endif
	}

	return oflags;
}
