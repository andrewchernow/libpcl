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

#include "_win32.h"
#include <pcl/stat.h>

mode_t
ipcl_win32_rights_to_mode(DWORD rights, int type)
{
	mode_t mode = 0;

	switch(type)
	{
		case S_IRUSR:
			if(rights & NT_READ_TO_MODE)
				mode |= S_IRUSR;

			if(rights & NT_WRITE_TO_MODE)
				mode |= S_IWUSR;

			if(rights & NT_EXECUTE_TO_MODE)
				mode |= S_IXUSR;

			break;

		case S_IRGRP:
			if(rights & NT_READ_TO_MODE)
				mode |= S_IRGRP;

			if(rights & NT_WRITE_TO_MODE)
				mode |= S_IWGRP;

			if(rights & NT_EXECUTE_TO_MODE)
				mode |= S_IXGRP;

			break;

		case S_IROTH:
			if(rights & NT_READ_TO_MODE)
				mode |= S_IROTH;

			if(rights & NT_WRITE_TO_MODE)
				mode |= S_IWOTH;

			if(rights & NT_EXECUTE_TO_MODE)
				mode |= S_IXOTH;

			break;
	}

	return mode;
}
