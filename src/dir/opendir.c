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

#include "_dir.h"
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/file.h>

#ifdef PCL_WINDOWS
#	include <pcl/string.h>
#endif

pcl_dir_t *
pcl_opendir(const pchar_t *path)
{
	if(strempty(path))
		return R_SETERR(NULL, PCL_EINVAL);

	int len;
	pchar_t *fullpath = pcl_realpath(path, &len);

	if(!fullpath)
		return R_TRC(NULL);

	pcl_dir_t *dir = pcl_malloc(sizeof(pcl_dir_t));
	dir->path = fullpath;
	dir->pathlen = len;

#ifdef PCL_WINDOWS
	dir->path = pcl_realloc(dir->path, (dir->pathlen + 3) * sizeof(pchar_t));

	/* windows requires a "\*" at the end of path */
	if(dir->path[len - 1] != PCL_PPATHSEPCHAR)
		dir->path[len++] = PCL_PPATHSEPCHAR;

	dir->path[len++] = _P('*');
	dir->path[len] = 0;

	/* FindExInfoBasic excludes short file names increasing performance. Also,
	 * FIND_FIRST_EX_LARGE_FETCH uses larger buffers increasing performance.
	 */
	dir->handle = FindFirstFileEx(dir->path, FindExInfoBasic, &dir->wfd,
		FindExSearchNameMatch, NULL, FIND_FIRST_EX_LARGE_FETCH);

	/* remove the trailing "\*" appended above */
	dir->path[dir->pathlen] = 0;

	if(dir->handle == INVALID_HANDLE_VALUE)
	{
		SETLASTERRMSG("Cannot open directory: %Ps", path);
		pcl_closedir(dir);
		return NULL;
	}

	dir->findnextfile = false;

#else
	dir->handle = opendir(dir->path);

	if(!dir->handle)
	{
		SETLASTERRMSG("Cannot open directory: %Ps", path);
		pcl_closedir(dir);
		return NULL;
	}
#endif

	return dir;
}
