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

#include "_dir.h"
#include <pcl/error.h>
#include <pcl/string.h>
#include <pcl/io.h>
#include <pcl/stat.h>
#include <pcl/alloc.h>

#ifdef PCL_WINDOWS
#	include "../file/_file.h"
#else
#	include <dirent.h>
#endif

bool
pcl_readdir(pcl_dir_t *dir, pcl_dirent_t *ent, pcl_stat_t *stbuf)
{
	if(!dir || !ent)
		return R_SETERR(false, PCL_EINVAL);

	memset(ent, 0, sizeof(*ent));
	pcl_err_osclear();

#ifdef PCL_WINDOWS
	/* false for the first pcl_readdir since pcl_opendir does FindFirstFileEx
	 * whose results are handled here
	 */
	if(!dir->findnextfile)
	{
		/* use FindNextFile from this point forward */
		dir->findnextfile = true;

		/* always skip "." and ".." */
		if(pcl_pcscmp(dir->wfd.cFileName, _P(".")) && pcl_pcscmp(dir->wfd.cFileName, _P("..")))
			goto FOUND_ENTRY;
	}

	while(true)
	{
		if(FindNextFile(dir->handle, &dir->wfd))
		{
			/* always skip "." and ".." */
			if(!pcl_pcscmp(dir->wfd.cFileName, _P(".")) || !pcl_pcscmp(dir->wfd.cFileName, _P("..")))
				continue;

			/* have an entry */
			break;
		}

		/* skip access denied. if FindNextFile is finished, error is ERROR_NO_MORE_FILES
		 * which maps to PCL_ENOMORE.
		 */
		if(pcl_oserrno != ERROR_ACCESS_DENIED)
		{
			SETLASTERR(); // success if GetLastError() == ERROR_NO_MORE_FILES
			return false;
		}
	}

	FOUND_ENTRY:;
	ent->type = PclDirentUnknown;
	ent->namlen = (uint16_t) pcl_pcslen(ent->name);
	pcl_pcsncpy(ent->name, countof(ent->name), dir->wfd.cFileName, ent->namlen);

	/* get path to entry */
	pchar_t *entpath;
	pcl_aspprintf(&entpath, _P("%Ps%/%Ps"), dir->path, ent->name);

	/* use GetFileType for fifo and chr types, need a win32 file HANDLE */
	pcl_file_t *file = pcl_file_open(entpath, PCL_O_RDONLY);
	pcl_free(entpath);

	if(file)
	{
		DWORD type = GetFileType(file->fd);

		if(type == FILE_TYPE_CHAR)
			ent->type = PclDirentChar;
		else if(type == FILE_TYPE_PIPE)
			ent->type = PclDirentFifo;

		pcl_file_close(file);
	}

	/* pcl_file_open failed or GetFileType was FILE_TYPE_DISK (disk file) or FILE_TYPE_UNKNOWN */
	if(ent->type == PclDirentUnknown)
	{
		bool symlink = (dir->wfd.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) &&
			dir->wfd.dwReserved0 == IO_REPARSE_TAG_SYMLINK;

		if(symlink)
			ent->type = PclDirentLink;
		else if(dir->wfd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			ent->type = PclDirentDir;
		else
			ent->type = PclDirentFile;
	}

#else
	while(true)
	{
#	ifdef PCL_DARWIN
		char entbuf[4096] = {0};
		struct dirent *e = (struct dirent *) entbuf, *result = NULL;
		int err = readdir_r(dir->handle, e, &result);
#	else // apparently, readdir_r is now deprecated, but not on darwin.
		errno = 0;
		struct dirent *e = readdir(dir->handle);
		struct dirent *result = e;
		int err = errno;
#	endif

		if(err)
			return R_SETOSERR(false, err);

		/* nothing left in directory */
		if(!result)
			return R_SETERR(false, PCL_ENOMORE);

		/* do not report these */
		if(!pcl_pcscmp(e->d_name, _P(".")) || !pcl_pcscmp(e->d_name, _P("..")))
			continue;

		/* got an entry */
		switch(e->d_type)
		{
			case DT_DIR:
				ent->type = PclDirentDir;
				break;

			case DT_REG:
				ent->type = PclDirentFile;
				break;

			case DT_LNK:
				ent->type = PclDirentLink;
				break;

			case DT_SOCK:
				ent->type = PclDirentSocket;
				break;

			case DT_CHR:
				ent->type = PclDirentChar;
				break;

			case DT_BLK:
				ent->type = PclDirentBlock;
				break;

			case DT_FIFO:
				ent->type = PclDirentFifo;
				break;

			default:
				ent->type = PclDirentUnknown;
		}

		ent->namlen = (uint16_t) strlen(e->d_name);
		pcl_pcsncpy(ent->name, countof(ent->name), e->d_name, ent->namlen);

		break;
	}
#endif

	/* caller requested an lstat on entry */
	if(stbuf)
	{
		pchar_t *path;
		int r = pcl_aspprintf(&path, _P("%Ps%/%Ps"), dir->path, ent->name);

		if(r < 0)
			return R_SETERR(false, -r);

		r = pcl_lstat(path, stbuf);
		pcl_free(path);

		if(r)
			return R_TRC(false);
	}

	return true;
}
