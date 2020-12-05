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

int
pcl_mkdirs(const tchar_t *_path, mode_t mode)
{
	tchar_t path[16 * 1024];
	tchar_t *path_pos;
	uint32_t save_err = pcl_oserrno;

	path_pos = pcl_tcscpy(path, countof(path), _path);

#ifdef PCL_WINDOWS
	/* Skip C:\, drive roots */
	if(path_pos[1] == _T(':'))
	{
		path_pos += 3;
	}
	/* \drive_root syntax */
	else if(path_pos[0] == PCL_TPATHSEPCHAR)
	{
		path_pos++;

		/* UNC \\server */
		if(path_pos[0] == PCL_TPATHSEPCHAR)
			path_pos++;
	}

#else
	/* Skip '/' root */
	if(path_pos[0] == PCL_TPATHSEPCHAR)
		path_pos++;
#endif

	while(true)
	{
		tchar_t *sep = pcl_tcschr(path_pos, PCL_TPATHSEPCHAR);

		if(sep)
			*sep = 0;

		if(pcl_mkdir(path, mode) && pcl_errno != PCL_EEXIST)
			return TRCMSG("failed to create directories: '%ts'", _path);

		if(!sep)
			break;

		*sep = PCL_TPATHSEPCHAR;
		path_pos = ++sep;
	}

	pcl_setoserrno(save_err);
	return 0;
}
