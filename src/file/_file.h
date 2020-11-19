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

#ifndef LIBPCL__UNISTD_H__
#define LIBPCL__UNISTD_H__

#include <pcl/file.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef PCL_WINDOWS
#	include <windows.h>

	/** used by pcl_file_t for windows. provides OVERLAPPED structure and
	 * some state in regard to pending and who requested it
	 */
	struct ipcl_win32_aio
	{
		OVERLAPPED ol;

		/** When zero, there are no pending I/O operations. When non-zero, there
		 * are pending I/O operations and this is set to the requester's thread-id.
		 */
		uint64_t pending;
	};
#else
#	include <fcntl.h>
#	include <unistd.h>
#endif

struct tag_pcl_file
{
	uint32_t flags;
	pcl_fd_t fd;
#ifdef PCL_WINDOWS
	struct ipcl_win32_aio aio_read;
	struct ipcl_win32_aio aio_write;
#endif
};

/* Allocates a new PCL file object and optionally allows specifying the
* backing file descriptor and any file flags: like PCL_FF_NONBLOCK,
* PCL_FF_APPEND, PCL_FF_LOCKED, etc... These arguments can be zero.
*/
pcl_file_t *ipcl_file_alloc(pcl_fd_t fd, int fflags);

int ipcl_file_open(pcl_file_t *file, const tchar_t *path, int flags, mode_t mode);

/** Initialize a preallocated file using a set of file-flags.
 * @param file
 * @param fflags
 * @return
 */
pcl_file_t *ipcl_file_init(pcl_file_t *file, int fflags);

/* You cannot set PCL_FF_OLOCK or PCL_FF_FLOCK with this call */
int ipcl_oflags_to_fflags(int oflags);

/* Convert PCL_O_* open flags to the system's (OS) open flags. There is some
 * lose during the conversion that pcl_open() handles internally. If using
 * this outside of pcl_open(), be aware of that.
 */
int ipcl_sysoflags(uint32_t pcl_oflags);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL__UNISTD_H__
