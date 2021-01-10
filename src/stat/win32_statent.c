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

#include "_stat.h"
#include "../time/_time.h"
#include "../win32/_win32.h"
#include <pcl/vector.h>
#include <pcl/event.h>
#include <pcl/thread.h>
#include <pcl/alloc.h>
#include <pcl/file.h>
#include <Shlwapi.h>
#include <io.h>

#define makedev(x, y)  ((dev_t)(((x) << 24) | (y)))

typedef struct
{
	pchar_t *root;
	int blksize;
} bsize_cache_t;

static pthread_key_t bsize_key;

static void convstat(pchar_t *path, pcl_stat_t *st, WIN32_FIND_DATA *wfd, int flags, bool is_root);

static void
bsize_veccleanup(pcl_vector_t *v, void *elem)
{
	bsize_cache_t *b = (bsize_cache_t *) elem;
	pcl_free_safe(b->root);
	UNUSED(v);
}

static void
bsize_thread_destroy(void *a)
{
	pcl_vector_free((pcl_vector_t *) a);
}

void
ipcl_win32_stat_handler(uint32_t which, void *data)
{
	UNUSED(data);

	if(which == PCL_EVENT_INIT)
		pcl_tls_alloc(&bsize_key, bsize_thread_destroy);

	if(which == PCL_EVENT_INIT || which == PCL_EVENT_THREADINIT)
		pcl_tls_set(bsize_key, pcl_vector(2, sizeof(bsize_cache_t), bsize_veccleanup));
}

int
ipcl_statent(const pchar_t *_path, int fd, pcl_stat_t *buf, int flags)
{
	HANDLE hfind;
	WIN32_FIND_DATA wfd;

	if(strempty(_path) && fd < 0)
		return BADARG();

	memset(&wfd, 0, sizeof(wfd));

	/* this is a windows fstat */
	if(fd > -1)
	{
		BY_HANDLE_FILE_INFORMATION info = {0};

		if(!GetFileInformationByHandle((HANDLE) _get_osfhandle(fd), &info))
			return SETLASTERR();

		/* no stat buf, caller is just doing an exists check */
		if(!buf)
			return 0;

		wfd.dwFileAttributes = info.dwFileAttributes;
		wfd.ftCreationTime = info.ftCreationTime;
		wfd.ftLastAccessTime = info.ftLastAccessTime;
		wfd.ftLastWriteTime = info.ftLastWriteTime;
		wfd.nFileSizeHigh = info.nFileSizeHigh;
		wfd.nFileSizeLow = info.nFileSizeLow;

		convstat(NULL, buf, &wfd, flags, false);
		return 0;
	}

	pchar_t *path = pcl_realpath(_path, NULL);

	if(!path)
		return TRC();

	/* UNC Server: \\server */
	if(PathIsUNCServer(path))
	{
		wfd.dwFileAttributes = GetFileAttributes(path);

		if(wfd.dwFileAttributes == INVALID_FILE_ATTRIBUTES)
		{
			SETLASTERRMSG("cannot get information about '%Ps'", path);
			pcl_free(path);
			return -1;
		}

		if(buf)
			convstat(path, buf, &wfd, flags, false);

		pcl_free(path);
		return 0;
	}

	/* Roots: C:\, \, \\server\share (PathIsUNCServerShare) */
	if(PathIsRoot(path))
	{
		WIN32_FILE_ATTRIBUTE_DATA attr;

		if(!GetFileAttributesEx(path, GetFileExInfoStandard, &attr))
		{
			SETLASTERRMSG("cannot get information about '%Ps'", path);
			pcl_free(path);
			return -1;
		}

		if(buf)
		{
			wfd.dwFileAttributes = attr.dwFileAttributes;
			wfd.ftCreationTime = attr.ftCreationTime;
			wfd.ftLastAccessTime = attr.ftLastAccessTime;
			wfd.ftLastWriteTime = attr.ftLastWriteTime;
			wfd.nFileSizeHigh = attr.nFileSizeHigh;
			wfd.nFileSizeLow = attr.nFileSizeLow;
			convstat(path, buf, &wfd, flags, true);
		}

		pcl_free(path);
		return 0;
	}

	/* Regular file system object */
	hfind = FindFirstFile(path, &wfd);

	if(hfind == INVALID_HANDLE_VALUE)
	{
		SETLASTERRMSG("cannot get information about '%Ps'", path);
		pcl_free(path);
		return -1;
	}

	FindClose(hfind);

	if(buf)
		convstat(path, buf, &wfd, flags, false);

	pcl_free(path);
	return 0;
}

static void
convstat(pchar_t *path, pcl_stat_t *st, WIN32_FIND_DATA *wfd, int flags, bool is_root)
{
	st->nlink = 1;

	/* default is SECURITY_NULL_SID_AUTHORITY: ie Unknown */
	pcl_strcpy(st->uid, countof(st->gid), "S-1-0-0");
	pcl_strcpy(st->gid, countof(st->gid), "S-1-0-0");

	/* get unix permission bits from the DACL.  If that fails, use boiler
	 * plate hack from Microsoft's CRT library.
	 */
	if((flags & PCL_STAT_SKIP_WIN32OWNER) || !path ||
		 ipcl_win32_file_security(path, &st->mode, &st->uid, &st->gid))
	{
		if(wfd->dwFileAttributes & FILE_ATTRIBUTE_READONLY)
			st->mode |= (_S_IREAD + (_S_IREAD >> 3) + (_S_IREAD >> 6));
		else
			st->mode |= ((_S_IREAD | _S_IWRITE) + ((_S_IREAD | _S_IWRITE) >> 3) +
									 ((_S_IREAD | _S_IWRITE) >> 6));
	}

	/* Use drive letter as device id */
	if(!strempty(path) && path[1] == _P(':'))
	{
		int drive = pcl_topupper(path[0]);

		st->dev = makedev(FILE_TYPE_DISK, drive);

		if(is_root)
			st->mode |= S_IFBLK;
	}

	if((flags & PCL_STAT_LINK) && (wfd->dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
			(wfd->dwReserved0 == IO_REPARSE_TAG_SYMLINK))
		st->mode |= S_IFLNK;

	/* only set dir or reg if link is not already set */
	if(!S_ISLNK(st->mode))
	{
		if(wfd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			st->mode |= S_IFDIR;
		else
			st->mode |= S_IFREG;
	}

	/* set st_size to the number of used bytes */
	if(is_root)
	{
		ULARGE_INTEGER btot, bfree;
		if(path && GetDiskFreeSpaceEx(path, NULL, &btot, &bfree))
			st->size = btot.QuadPart - bfree.QuadPart;
	}
	else
	{
		st->size = ((int64_t) wfd->nFileSizeHigh * ((int64_t) MAXDWORD + 1)) +
							 (int64_t) wfd->nFileSizeLow;
	}

	/* File Times */
	st->atime = ipcl_win32_ftime_to_pcl(&wfd->ftLastAccessTime);
	st->mtime = ipcl_win32_ftime_to_pcl(&wfd->ftLastWriteTime);
	st->btime = ipcl_win32_ftime_to_pcl(&wfd->ftCreationTime);
	st->ctime = st->mtime; /* changed time resembles mtime the most */

	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE)
		st->flags |= SF_ARCHIVED;
	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_HIDDEN)
		st->flags |= UF_HIDDEN;
	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED)
		st->flags |= UF_COMPRESSED;
	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_ENCRYPTED)
		st->flags |= UF_ENCRYPTED;
	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_SPARSE_FILE)
		st->flags |= UF_SPARSE;
	if(wfd->dwFileAttributes & FILE_ATTRIBUTE_SYSTEM)
		st->flags |= UF_SYSTEM;

	if(!path)
		return;

	/* GetDiskFreeSpace() is expensive which is why we cache each root path's block size as we
	 * see them.  We do this per thread to avoid requiring locks.
	 */

	st->blksize = 4096;

	if(!PathIsRelative(path) && PathStripToRoot(path))
	{
		/* GetDiskFreeSpace needs a trailing slash */
		size_t path_len = pcl_pcslen(path);

		if(path[path_len - 1] != PCL_PPATHSEPCHAR)
		{
			path[path_len++] = L'\\';
			path[path_len] = 0;
		}

		/* check cache */
		int i;
		pcl_vector_t *bsize_cache = pcl_tls_get(bsize_key);

		for(i = 0; i < bsize_cache->count; i++)
		{
			bsize_cache_t *bs = pcl_vector_get(bsize_cache, i);

			if(!pcl_pcsicmp(bs->root, path))
			{
				st->blksize = bs->blksize;
				break;
			}
		}

		/* not in cache yet */
		if(i == bsize_cache->count)
		{
			DWORD a, b, sectors_per_cluster, bytes_per_sec;

			if(GetDiskFreeSpace(path, &sectors_per_cluster, &bytes_per_sec, &a, &b))
			{
				bsize_cache_t elem;

				elem.root = pcl_pcsdup(path);
				st->blksize = elem.blksize = (int) (sectors_per_cluster * bytes_per_sec);
				pcl_vector_push(bsize_cache, &elem);
			}
		}
	}

	st->dsize = roundup(st->size, st->blksize);

	st->blocks = st->dsize / 512;
	if(st->blocks <= 0)
		st->blocks = 1;
}
