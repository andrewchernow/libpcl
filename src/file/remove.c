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

#include <pcl/file.h>
#include <pcl/io.h>
#include <pcl/error.h>

#ifdef PCL_WINDOWS
#	include <pcl/dir.h>
#	include <pcl/stat.h>
#endif

int pcl_remove(const pchar_t *pathname)
{
	if(strempty(pathname))
		return BADARG();

#ifdef PCL_WINDOWS
	pcl_stat_t st;

	if(pcl_stat(pathname, &st))
		return TRC();

	if(S_ISDIR(st.mode))
	{
		if(pcl_rmdir(pathname))
			return TRC();
	}
	else if(!S_ISBLK(st.mode) && !S_ISCHR(st.mode))
	{
		if(pcl_unlink(pathname))
			return TRC();
	}

#else
	if(remove(pathname))
		return TRC();
#endif

	return 0;
}
