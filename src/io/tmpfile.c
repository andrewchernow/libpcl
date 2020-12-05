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

#include <pcl/crypto.h>
#include <pcl/string.h>
#include <pcl/stat.h>
#include <pcl/alloc.h>
#include <pcl/file.h>
#include <pcl/limits.h>
#include <pcl/error.h>
#include <pcl/io.h>
#include <pcl/process.h>
#include <fcntl.h>

#define RANDLEN 8

// combo of tmpfile and tempnam
FILE *
pcl_tmpfile(const tchar_t *dir, const tchar_t *prefix, const tchar_t *suffix,
	tchar_t **pathp, bool keep)
{
	tchar_t *tmpdirs[4];
	size_t prefix_len = prefix ? pcl_tcslen(prefix) : 0;
	size_t suffix_len = suffix ? pcl_tcslen(suffix) : 0;

	if(prefix_len + suffix_len > PCL_MAXNAME)
		return R_SETERRMSG(NULL, PCL_EINVAL, "prefix and suffix cannot exceed maximum "
																				 "file name length '%d'", PCL_MAXNAME);

	if(!prefix)
		prefix = _T("");
	if(!suffix)
		suffix = _T("");

	tmpdirs[0] = (tchar_t *) dir;

#ifdef PCL_WINDOWS
	tmpdirs[1] = pcl_getenv(_T("TMP"));
	tmpdirs[2] = pcl_getenv(_T("TEMP"));
#else
	tmpdirs[1] = pcl_getenv(_T("TMPDIR"));
	tmpdirs[2] = P_tmpdir;
#endif

	tmpdirs[3] = _T(".");

	tchar_t *path = NULL;
	pcl_file_t *file = NULL;
	size_t rand_len = PCL_MAXNAME - (prefix_len + suffix_len);
	int oflags = PCL_O_CREAT | PCL_O_EXCL | PCL_O_RDWR;

	rand_len = min(RANDLEN, rand_len);
	if(!keep)
		oflags |= PCL_O_TEMP;

	for(int i = 0; i < countof(tmpdirs); i++)
	{
		tchar_t *tmpdir = tmpdirs[i];

		if(!tmpdir)
			continue;

		int attempts = 0;
		pcl_stat_t st;
		uint8_t rand[RANDLEN];
		char randhex[(RANDLEN * 2) + 1];

		if(pcl_stat(tmpdir, &st) || !S_ISDIR(st.mode))
			continue;

		/* first +1 is path separator and 2nd +1 is NUL */
		size_t path_len = pcl_tcslen(tmpdir) + 1 + prefix_len + rand_len + suffix_len + 1;

		path = pcl_malloc(path_len * sizeof(tchar_t));

		do
		{
			/* generate random part of temp name */
			pcl_rand(rand, (int) rand_len);
			pcl_hex_encode(randhex, sizeof(randhex), rand, rand_len);

			/* create tmp path */
			pcl_stprintf(path, path_len, _T("%ts%/%ts%s%ts"), tmpdir, prefix, randhex, suffix);

			/* create tmp file */
			file = pcl_file_open(path, oflags, S_IRUSR | S_IWUSR);

			if(file)
				goto END_LOOPS;
		}
		while(++attempts < 10); // try 10 times then move to next tmpdir

		path = pcl_free(path);
	} // for tmpdirs

END_LOOPS:

	pcl_free_safe(tmpdirs[1]);
#ifdef PCL_WINDOWS
	pcl_free_safe(tmpdirs[2]);
#endif

	if(!file)
		return R_SETERR(NULL, PCL_ENOTFOUND);

	if(pathp)
		*pathp = path;
	else
		pcl_free(path);

	/* convert to a FILE stream */
	FILE *fp = pcl_fdopen(file);

	if(!fp)
	{
		pcl_file_close(file);
		return R_TRC(NULL);
	}

	return fp;
}
