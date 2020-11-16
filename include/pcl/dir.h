
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

#ifndef LIBPCL_DIRENT_H
#define LIBPCL_DIRENT_H

#include <pcl/types.h>

#ifdef PCL_UNIX
	#include <dirent.h>
#else
	#define DT_UNKNOWN 0
	#define DT_FIFO    1
	#define DT_CHR     2
	#define DT_DIR     4
	#define DT_BLK     6
	#define DT_REG     8
	#define DT_LNK    10
	#define DT_SOCK   12
	#define DT_WHT    14
#endif

#ifdef __cplusplus
extern "C" {
#endif

#ifndef DIRBLKSIZ
	#define DIRBLKSIZ 1024
#endif

typedef struct
{
	uint8_t type;
	uint16_t namlen;
	tchar_t name[DIRBLKSIZ];
} pcl_dirent_t;

PCL_EXPORT DIR *pcl_opendir(const tchar_t *path);

PCL_EXPORT bool pcl_readdir(DIR *dp, pcl_dirent_t *ent);

PCL_EXPORT void pcl_closedir(DIR *dp);

/** Remove a directory, which must be empty
 * @param path directory to remove
 * @return 0 for success and -1 on error
 * @see pcl_rmdir_r
 */
PCL_EXPORT int pcl_rmdir(const tchar_t *path);

/** Recursively remove a directory
 * @param path directory to remove
 * @return 0 for success and -1 on error
 */
PCL_EXPORT int pcl_rmdir_r(const tchar_t *path);

/** Change the current working directory.
 * @param path new working directory
 * @return 0 for success and -1 on error
 */
PCL_EXPORT int pcl_chdir(const tchar_t *path);

/**
 * @param path
 * @param mode
 * @return
 */
PCL_EXPORT int pcl_mkdir(const tchar_t *path, mode_t mode);

/** Create a directory including any absent parents. Same as unix `mkdir -p /path/to/dir`
 * @param path
 * @param mode
 * @return
 */
PCL_EXPORT int pcl_mkdirs(const tchar_t *path, mode_t mode);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_DIRENT_H
