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

#ifndef LIBPCL_UNISTD_H__
#define LIBPCL_UNISTD_H__

#include <pcl/types.h>

#ifdef PCL_WINDOWS
	#define PCL_RDLOCK 1 /* shared or read lock */
	#define PCL_UNLOCK 2 /* unlock */
	#define PCL_WRLOCK 3 /* exclusive or write lock */
#else
	#define PCL_RDLOCK F_RDLCK
	#define PCL_UNLOCK F_UNLCK
	#define PCL_WRLOCK F_WRLCK
#endif

/* STDIO fixed FD values */
#ifndef STDIN_FILENO
	#define STDIN_FILENO 0
#endif
#ifndef STDOUT_FILENO
	#define STDOUT_FILENO 1
#endif
#ifndef STDERR_FILENO
	#define STDERR_FILENO 2
#endif

#define PCL_O_RDONLY     0x00000000     /* open for reading only */
#define PCL_O_WRONLY     0x00000001     /* open for writing only */
#define PCL_O_RDWR       0x00000002     /* open for reading and writing */
#define PCL_O_ACCMODE    0x00000003     /* mask for above modes */
#define PCL_O_CREAT      0x00000008     /* create and open file */
#define PCL_O_APPEND     0x00000010     /* set the append mode (all writes at EOF */
#define PCL_O_TRUNC      0x00000020     /* truncate to zero length */
#define PCL_O_EXCL       0x00000040     /* open only if file doesn't already exist */
#define PCL_O_NONBLOCK   0x00000080     /* do not block on open or for data to become available */
#define PCL_O_CLOEXEC    0x00000100     /* close fd when exec() is invoked */
#define PCL_O_DIRECTORY  0x00000200     /* If not a directory, the open will fail */
#define PCL_O_NOFOLLOW   0x00000400     /* Do not follow symlinks, the open will fail */
#define PCL_O_SYNC       0x00000800     /* file is opened for synchronous I/O */
#define PCL_O_SHLOCK     0x00001000     /* share lock file, read lock */
#define PCL_O_EXLOCK     0x00002000     /* exclusive lock the file, read/write lock */
#define PCL_O_BACKUP     0x00004000     /* win32 only, sets FILE_FLAG_BACKUP_SEMANTICS
                                           and possibly other things in future */
#define PCL_O_NOATIME    0x00008000     /* linux only */
#define PCL_O_NDELAY     PCL_O_NONBLOCK
#define PCL_O_NOINHERIT  PCL_O_CLOEXEC

/* pcl_file_t.flags */
#define PCL_FF_FLOCK     0x0001 /* locked explicitly via pcl_flock */
#define PCL_FF_NONBLOCK  0x0002
#define PCL_FF_APPEND    0x0004
#define PCL_FF_READ      0x0008
#define PCL_FF_WRITE     0x0010
#define PCL_FF_OLOCK     0x0020 /* locked implicitly by pcl_open O_SHLOCK|O_EXLOCK */
#define PCL_FF_RDWR      (PCL_FF_READ | PCL_FF_WRITE)

#ifdef __cplusplus
extern "C" {
#endif

PCL_EXPORT pcl_file_t *pcl_open(const tchar_t *path, int oflags, ...);

PCL_EXPORT int pcl_read(pcl_file_t *file, void *buf, size_t count);

PCL_EXPORT int pcl_write(pcl_file_t *file, const void *data, size_t count);

PCL_EXPORT int pcl_tryread(pcl_file_t *file, void *buf, size_t count, int ms_timeout);

PCL_EXPORT int pcl_trywrite(pcl_file_t *file, const void *data, size_t count, int ms_timeout);

PCL_EXPORT off_t pcl_seek(pcl_file_t *file, off_t offset, int whence);

PCL_EXPORT void pcl_close(pcl_file_t *file);

/** Lock a file with exclusive or shared access or unlock a file.
 * @param file
 * @param operation PCL_WRLOCK exclusive, PCL_RDLOCK shared, PCL_UNLOCK unlock
 * @return 0 for success and -1 on error
 */
PCL_EXPORT int pcl_flock(pcl_file_t *file, int operation);

/* Get the absolute path for a file descriptor.
 * @param fd file descriptor. For windows, this is a CRT fd, not a HANDLE.
 * @param out pointer to a tchar_t pointer to receive the result which must be freed
 * by caller.
 * @return -1 on error number of characters written to out on success
 */
PCL_EXPORT int pcl_fdpath(int fd, tchar_t **out);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_UNISTD_H__
