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

#include "_time.h"
#include "../file/_file.h"
#include <pcl/error.h>

int
pcl_utimes(const pchar_t *path, pcl_time_t *atime, pcl_time_t *mtime, pcl_time_t *btime)
{
	FILETIME filetimes[3];
	FILETIME *ft_btime = NULL, *ft_atime = NULL, *ft_mtime = NULL;
	pcl_file_t *file = pcl_file_open(path, PCL_O_BACKUP | PCL_O_WRONLY);

	if(!file)
		return TRC();

	if(btime)
	{
		ft_btime = &filetimes[0];
		*ft_btime = ipcl_win32_pcl_to_ftime(*btime);
	}

	if(atime)
	{
		ft_atime = &filetimes[1];
		*ft_atime = ipcl_win32_pcl_to_ftime(*atime);
	}

	if(mtime)
	{
		ft_mtime = &filetimes[2];
		*ft_mtime = ipcl_win32_pcl_to_ftime(*mtime);
	}

	BOOL b = SetFileTime(file->fd, ft_btime, ft_atime, ft_mtime);
	pcl_file_close(file);

	if(!b)
		return SETLASTERR();

	return 0;
}
