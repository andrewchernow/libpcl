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

#include <pcl/stat.h>

char *
pcl_strmode(char *out, size_t out_size, mode_t mode)
{
	char *out_save = out;

	if(out)
		*out = 0;

	if(out && out_size >= 11)
	{
		switch(mode & S_IFMT)
		{
			case S_IFDIR:  *out++ = 'd'; break;
			case S_IFREG:  *out++ = '-'; break;
			case S_IFLNK:  *out++ = 'l'; break;
			case S_IFBLK:  *out++ = 'b'; break;
			case S_IFCHR:  *out++ = 'c'; break;
			case S_IFIFO:  *out++ = 'p'; break;
			case S_IFSOCK: *out++ = 's'; break;
			default:       *out++ = '?'; break;
		}

		/* owner */
		*out++ = mode & S_IRUSR ? 'r' : '-';
		*out++ = mode & S_IWUSR ? 'w' : '-';

		/* setuid */
		if(mode & S_ISUID)
			*out++ = mode & S_IXUSR  ? 's' : 'S';
		else
			*out++ = mode & S_IXUSR ? 'x' : '-';

		/* group */
		*out++ = mode & S_IRGRP ? 'r' : '-';
		*out++ = mode & S_IWGRP ? 'w' : '-';

		/* setgid */
		if(mode & S_ISGID)
			*out++ = mode & S_IXGRP  ? 's' : 'S';
		else
			*out++ = mode & S_IXGRP ? 'x' : '-';

		/* other */
		*out++ = mode & S_IROTH ? 'r' : '-';
		*out++ = mode & S_IWOTH ? 'w' : '-';

		/* sticky bit */
		if(mode & S_ISVTX)
			*out++ = mode & S_IXUSR ? 't' : 'T';
		else
			*out++ = mode & S_IXOTH ? 'x' : '-';

		*out = 0;
	}

	return out_save;
}
