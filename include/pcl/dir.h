
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

#ifndef LIBPCL_DIRENT_H
#define LIBPCL_DIRENT_H

/** @defgroup dir Directory Management
 * The directory API provides a suite of function for managing and traversing  directories.
 * #### Basic Usage
 * @code
 * #include <pcl/init.h>
 * #include <pcl/error.h>
 * #include <pcl/dir.h>
 * #include <pcl/io.h>
 *
 * int pcl_tmain(int argc, tchar_t *targv)
 * {
 *   // always first
 *   pcl_init();
 *
 *   if(argc < 2)
 *     PANIC("Missing directory argument", 0);
 *
 *   pcl_dir_t *dir = pcl_opendir(targv[1]);
 *
 *   if(!dir)
 *     PANIC("Failed to open directory: '%ts'", targv[1]);
 *
 *   pcl_dirent_t ent;
 *   const tchar_t *dirpath = pcl_dir_path(dir, NULL);
 *
 *   while(pcl_readdir(dir, &ent, NULL))
 *   {
 *     char *type;
 *
 *     switch(ent.type)
 *     {
 *       case PclDirentFile:   type = "file"; break;
 *       case PclDirentDir:    type = "directory"; break;
 *       case PclDirentFifo:   type = "named pipe"; break;
 *       case PclDirentLink:   type = "symbolic link"; break;
 *       case PclDirentChar:   type = "character device"; break;
 *       case PclDirentBlock:  type = "block device"; break;
 *       case PclDirentSocket: type = "unix domain socket"; break;
 *       default:              type = "unknown";
 *     }
 *
 *     // %/ is replaced with the platform-specific path separator
 *     pcl_printf("%s] %ts%/%ts\n", type, dirpath, ent.name);
 *   }
 *
 *   pcl_closedir(dir);
 *
 *   // when readdir is done, it returns false and pcl_errno is PCL_ENOMORE
 *   if(pcl_errno != PCL_ENOMORE)
 *     PANIC("readdir failed: '%ts'", dirpath);
 *
 *   return 0;
 * }
 * @endcode
 * @{
 */
#include <pcl/types.h>

/** Directory entry types */
typedef enum
{
	/** Entry type could not be determined */
	PclDirentUnknown,
	/** Named pipe (FIFO) */
	PclDirentFifo,
	/** Character device */
	PclDirentChar,
	/** Directory */
	PclDirentDir,
	/** Block device */
	PclDirentBlock,
	/** Regular file */
	PclDirentFile,
	/** Symbolic link */
	PclDirentLink,
	/** Unix domain socket */
	PclDirentSocket
} pcl_dirent_type_t;


#ifdef __cplusplus
extern "C" {
#endif

/** Directory entry object. */
typedef struct
{
	/** type of entry: PclDirentFile, PclDirentDir, etc */
	pcl_dirent_type_t type;
	/** character length (excluding NUL) of the entry's file name */
	uint16_t namlen;
	/** entry's file name */
	tchar_t name[512];
} pcl_dirent_t;

/** Open a directory.
 * @param path directory path. Internally, this is converted to an absolute path and can be
 * retreived using ::pcl_dir_path
 * @return pointer to a directory handle or \c NULL on error
 */
PCL_EXPORT pcl_dir_t *pcl_opendir(const tchar_t *path);

/** Get a directory handle's path.
 * @note the returned path is always absolute.
 * @param dir pointer to a directory handle as returned by ::pcl_opendir
 * @param lenp If not \c NULL, the path character length will be written here (excluding NUL)
 * @return directory handle's path or \c NULL on error
 */
PCL_EXPORT const tchar_t *pcl_dir_path(pcl_dir_t *dir, int *lenp);

/** Read an entry from a directory handle.
 * @param dir pointer to a directory handle as returned by ::pcl_opendir
 * @param ent pointer to an entry buffer to receive the results
 * @param st If not \c NULL, a ::pcl_lstat will be executed for the entry. In most cases,
 * the pcl_dirent_t.type can be used to avoid an expensive ::pcl_lstat.
 * @return true on success and false on error. When there are no more entries, this returns
 * false and sets the pcl_errno to PCL_ENOMORE.
 */
PCL_EXPORT bool pcl_readdir(pcl_dir_t *dir, pcl_dirent_t *ent, pcl_stat_t *st);

/** Close a directory handle.
 * @param dir pointer to a directory handle as returned by ::pcl_opendir
 */
PCL_EXPORT void pcl_closedir(pcl_dir_t *dir);

/** Remove a directory.
 * @warning The directory must be empty.
 * @param path directory to remove
 * @return 0 for success and -1 on error
 * @see pcl_rmdir_r
 */
PCL_EXPORT int pcl_rmdir(const tchar_t *path);

/** Recursively remove a directory
 * @param path directory to remove
 * @return 0 for success and -1 on error
 * @see pcl_rmdir
 */
PCL_EXPORT int pcl_rmdir_r(const tchar_t *path);

/** Change the current working directory.
 * @param path new working directory
 * @return 0 for success and -1 on error
 */
PCL_EXPORT int pcl_chdir(const tchar_t *path);

/** Create a directory.
 * @param path directory path
 * @param mode permission bits
 * @return 0 on success and -1 on error
 */
PCL_EXPORT int pcl_mkdir(const tchar_t *path, mode_t mode);

/** Create a directory including any absent parents. Same as unix `mkdir -p /path/to/dir`
 * @param path directory path
 * @param mode permission bits, which are also applied to any parent created
 * @return 0 on success and -1 on error
 */
PCL_EXPORT int pcl_mkdirs(const tchar_t *path, mode_t mode);

#ifdef __cplusplus
}
#endif

/** @} */
#endif //LIBPCL_DIRENT_H
