
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

#ifndef LIBPCL_STAT_H
#define LIBPCL_STAT_H

#include <pcl/time.h>
#include <sys/stat.h>

#ifdef PCL_WINDOWS
	#define sys_stat _wstat64
	#define sys_fstat _fstat64
	#define sys_lstat sys_stat
	typedef struct __stat64 sys_stat_t;
#else
	#define sys_stat stat
	#define sys_fstat fstat
	#define sys_lstat lstat
	typedef struct stat sys_stat_t;
#endif

/* flags for pcl_statent: ie. pcl_stat, pcl_lstat and pcl_fstat */
#define PCL_STAT_LINK 0x01

/* skip looking up entry owner: SIDs and ACLs. Very expensive on Windows and
 * rarely needed after a stat call. This is for WIN32 only, thus the name.
 * No effect on non-Windows systems. The pcl_stat_t.mode will contain boiler
 * plate permission bits. If you need precise perms, do not use this flag.
 */
#define PCL_STAT_SKIP_WIN32OWNER 0x10

#define pcl_stat(path,stbuf)  pcl_statent(path, -1, stbuf, 0)
#define pcl_fstat(fd,stbuf)   pcl_statent(NULL, fd, stbuf, 0)
#define pcl_lstat(path,stbuf) pcl_statent(path, -1, stbuf, PCL_STAT_LINK)


/* ---------------------------------------------------------------------------
 * file type bits
 */

#ifndef S_IFMT
	#define S_IFMT 0170000 /* file type mask */
#endif
#ifndef S_IFSOCK
	#define S_IFSOCK 0140000
#endif
#ifndef S_IFLNK
	#define S_IFLNK 0120000
#endif
#ifndef S_IFREG
	#define S_IFREG 0100000
#endif
#ifndef S_IFBLK
	#define S_IFBLK 0060000
#endif
#ifndef S_IFDIR
	#define S_IFDIR 0040000
#endif
#ifndef S_IFCHR
	#define S_IFCHR 0020000
#endif
#ifndef S_IFIFO
	#define S_IFIFO 0010000
#endif


/* ---------------------------------------------------------------------------
 * file mode bits, least significant 9 bits are permission bits
 */

#define S_IFMOD   07777  /* mode bits: mask for file type & perms, pcl extension */
#ifndef S_ISUID
	#define S_ISUID 04000  /* set-uid bit */
#endif
#ifndef S_ISGID
	#define S_ISGID 02000  /* set-gid bit */
#endif
#ifndef S_ISVTX
	#define S_ISVTX 01000  /* sticky bit */
#endif
#ifndef S_IRWXU
	#define S_IRWXU 00700
#endif
#ifndef S_IRUSR
	#define S_IRUSR 00400
#endif
#ifndef S_IWUSR
	#define S_IWUSR 00200
#endif
#ifndef S_IXUSR
	#define S_IXUSR 00100
#endif
#ifndef S_IRWXG
	#define S_IRWXG 00070
#endif
#ifndef S_IRGRP
	#define S_IRGRP 00040
#endif
#ifndef S_IWGRP
	#define S_IWGRP 00020
#endif
#ifndef S_IXGRP
	#define S_IXGRP 00010
#endif
#ifndef S_IRWXO
	#define S_IRWXO 00007
#endif
#ifndef S_IROTH
	#define S_IROTH 00004
#endif
#ifndef S_IWOTH
	#define S_IWOTH 00002
#endif
#ifndef S_IXOTH
	#define S_IXOTH 00001
#endif
/* file type macro helpers */
#ifndef S_ISLNK
	#define S_ISLNK(m)	(((m) & S_IFMT) == S_IFLNK)
#endif
#ifndef S_ISREG
	#define S_ISREG(m)	(((m) & S_IFMT) == S_IFREG)
#endif
#ifndef S_ISDIR
	#define S_ISDIR(m)	(((m) & S_IFMT) == S_IFDIR)
#endif
#ifndef S_ISCHR
	#define S_ISCHR(m)	(((m) & S_IFMT) == S_IFCHR)
#endif
#ifndef S_ISBLK
	#define S_ISBLK(m)	(((m) & S_IFMT) == S_IFBLK)
#endif
#ifndef S_ISFIFO
	#define S_ISFIFO(m)	(((m) & S_IFMT) == S_IFIFO)
#endif
#ifndef S_ISSOCK
	#define S_ISSOCK(m)	(((m) & S_IFMT) == S_IFSOCK)
#endif

#ifndef UF_NODUMP
	#define UF_NODUMP      0x00000001 /* do not dump file */
#endif
#ifndef UF_IMMUTABLE
	#define UF_IMMUTABLE   0x00000002 /* file may not be changed */
#endif
#ifndef UF_APPEND
	#define UF_APPEND      0x00000004 /* writes to file may only append */
#endif
#ifndef UF_OPAQUE
	#define UF_OPAQUE      0x00000008	/* directory is opaque wrt. union */
#endif
#ifndef UF_NOUNLINK
	#define UF_NOUNLINK	   0x00000010 /* file may not be removed or renamed */
#endif
#ifndef UF_COMPRESSED
	#define UF_COMPRESSED  0x00000020	/* file is compressed */
#endif
#ifndef UF_TRACKED
	#define UF_TRACKED     0x00000040	/* file renames and deletes are tracked */
#endif

#define UF_ENCRYPTED     0x00000080 /* item is encrypted */
#define UF_SPARSE        0x00000100 /* item is a sparse file */
#define UF_SYSTEM        0x00000200 /* item used exclusively by OS */

#ifndef UF_HIDDEN
	#define UF_HIDDEN	     0x00008000	/* item is hidden */
#endif
#ifndef SF_ARCHIVED
	#define SF_ARCHIVED	   0x00010000	/* file is archived */
#endif
#ifndef SF_IMMUTABLE
	#define SF_IMMUTABLE	 0x00020000	/* file may not be changed */
#endif
#ifndef SF_APPEND
	#define SF_APPEND	     0x00040000	/* writes to file may only append */
#endif
#ifndef SF_NOUNLINK
	#define SF_NOUNLINK	   0x00100000 	/* file may not be removed or renamed */
#endif
#ifndef SF_SNAPSHOT
	#define SF_SNAPSHOT    0x00200000 	/* snapshot inode */
#endif


#ifdef __cplusplus
extern "C" {
#endif

struct tag_pcl_stat
{
	dev_t      dev;        /* ID of device containing file. on windows, the major
	                         is FILE_TYPE_DISK and minor is uppercase ascii
	                         value of the drive letter (drive paths only).
	                         If not, this is zero. Example: "1,67" 67=C:\.  Use
	                         the major()/minor() macros to extract numbers. */
	ino_t      ino;        /* inode number */
	mode_t     mode;       /* permissions and entry type as a bit mask */
	nlink_t    nlink;      /* number of hard links */
	uid_t      uid;        /* user ID of owner, on windows this is a
	                         SID string: ex. 'S-1-5-32-544' */
	gid_t      gid;        /* group ID of owner, on windows this is a
		                       SID string: ex. 'S-1-5-32-544' */
	dev_t      rdev;       /* device ID (if special file) */
	int64_t    size;       /* total size in bytes of entry's contents */
	int64_t    dsize;      /* total size in bytes of the entry's contents, including
	                         the overhead incurred by file system block size */
	int        blksize;    /* block size for file system I/O */
	uint64_t   blocks;     /* number of 512B blocks allocated */
	pcl_time_t atime;      /* time of last access */
	pcl_time_t mtime;      /* time of last modification */
	pcl_time_t ctime;      /* time of last status change (same as mtime on windows) */
	pcl_time_t btime;      /* creation (birth) time (no linux support until kernel 4.11) */

	/* UF_xxx and SF_xxx flags (see BSD chflags()).  On Darwin, these are
	 * directly lifted from st_flags or ATTR_CMN_FLAGS.  On Windows,
	 * WIN32_FIND_DATA.dwFileAttributes are translated.  On Linux pre 4.11, only
	 * UF_HIDDEN is set when the file name begins with a dot '.'. If >= 4.11, statx
	 * stx_attributes are used to populate this value.
	 */
	uint32_t   flags;
};

/** Get file or directory status. This is normally not used directly. Instead, pcl_stat,
 * pcl_fstat and pcl_lstat macros are used.
 * @param path filesystem entry to stat. If NULL, the fd argument must be valid.
 * @param fd file descriptor representing the entry to stat. If < 0, path is expected to be valid.
 * @param buf output stat buffer, this can be NULL in which case this function is merely an
 * "exists" check.
 * @param flags PCL_STAT_xxx flags such as PCL_STAT_LINK incicates this should perform an lstat.
 * @return on success 0 and -1 on error
 */
PCL_PUBLIC int pcl_statent(const pchar_t *path, int fd, pcl_stat_t *buf, int flags);

PCL_PUBLIC char *pcl_strmode(char *out, size_t out_size, mode_t mode);
PCL_PUBLIC char *pcl_strflags(char *out, size_t len, uint32_t flags);

#ifdef __cplusplus
}
#endif

#endif
