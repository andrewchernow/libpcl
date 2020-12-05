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

#include "_dir.h"
#include <pcl/error.h>
#include <pcl/string.h>
#include <pcl/file.h>

static int
rmdir_r(tchar_t *path, size_t size)
{
	int path_len;
	pcl_dirent_t ent;
	pcl_dir_t *dir = pcl_opendir(path);

	if(!dir)
		return TRC();

	path_len = (int) pcl_tcslen(path);
	path[path_len] = PCL_TPATHSEPCHAR;

	while(pcl_readdir(dir, &ent, NULL))
	{
		/* append entry name */
		pcl_tcscpy(path + path_len + 1, size - path_len - 1, ent.name);

		/* recurse */
		if(ent.type == PclDirentDir)
		{
			if(rmdir_r(path, size))
			{
				pcl_closedir(dir);
				path[path_len] = 0;
				return TRC();
			}
		}
		else if(pcl_unlink(path))
		{
			pcl_closedir(dir);
			path[path_len] = 0;
			return TRC();
		}
	}

	pcl_closedir(dir);
	path[path_len] = 0;

	/* readdir failed */
	if(pcl_errno != PCL_ENOMORE)
		return TRC();

	if(pcl_rmdir(path))
		return TRC();

	return 0;
}

int
pcl_rmdir_r(const tchar_t *path)
{
	tchar_t pbuf[32 * 1024];

	pcl_tcscpy(pbuf, countof(pbuf), path);

	if(rmdir_r(pbuf, countof(pbuf)))
		return TRC();

	return 0;
}
