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

#include "_process.h"
#include <pcl/error.h>
#include <pcl/string.h>

#ifdef PCL_DARWIN
#	include <libproc.h> // proc_pidpath
#endif

tchar_t *
pcl_proc_path(pid_t pid)
{
	if(pid < 0)
		pid = pcl_proc_self();

#ifdef PCL_LINUX
	char exe[80];
	char path[4096];

	sprintf(exe, "/proc/%d/exe", pid);

	/* readlink doesn't terminate buffer, so reserve a NUL */
	int len = (int) readlink(exe, path, sizeof(path) - 1);

#else // darwin
	char path[PROC_PIDPATHINFO_SIZE]; /* proc_pidpath is very picky about buf sizes! */
	int len = proc_pidpath(pid, path, PROC_PIDPATHINFO_SIZE);

	if(len == 0)
		len = -1;
#endif

	if(len < 0)
		return R_SETLASTERR(NULL);

	return pcl_tcsndup(path, len);
}

