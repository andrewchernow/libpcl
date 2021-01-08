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

#include "test.h"
#include <pcl/dir.h>
#include <pcl/error.h>
#include <pcl/stat.h>

#define TESTDIR _P("_testdir_")
#define TESTDIRA "_testdir_"

/**$ Open a directory */
TESTCASE(opendir)
{
	pcl_dir_t *dir = pcl_opendir(_P("."));
	ASSERT_NOTNULL(dir, "opendir failed");
	pcl_closedir(dir);
	return true;
}

/**$ Read entries from a directory */
TESTCASE(readdir)
{
	pcl_dirent_t ent;
	pcl_dir_t *dir = pcl_opendir(_P("."));

	while(pcl_readdir(dir, &ent, NULL));

	ASSERT_INTEQ(pcl_errno, PCL_ENOMORE, "readdir failed");
	pcl_closedir(dir);
	return true;
}

/**$ Create a directory */
TESTCASE(mkdir)
{
	int r = pcl_mkdir(TESTDIR, 0755);

	if(r)
	{
		(void) pcl_rmdir(TESTDIR);
		ASSERT_INTEQ(r, 0, "mkdir failed");
	}

	struct stat st;
	bool success = stat(TESTDIRA, &st) == 0 && S_ISDIR(st.st_mode);

	(void) pcl_rmdir(TESTDIR);

	ASSERT_TRUE(success, "missing test directory after mkdir");

	return success;
}

/**$ Create a directory that already exists */
TESTCASE(mkdir_exists)
{
	(void) pcl_mkdir(TESTDIR, 0755);

	int r = pcl_mkdir(TESTDIR, 0755);
	bool success = r < 0 && pcl_errno == PCL_EEXIST;

	(void) pcl_rmdir(TESTDIR);

	ASSERT_TRUE(success, "wrong error handling for existing directory");

	return success;
}

/**$ Create nested directory structure */
TESTCASE(mkdirs)
{
#define RMDIRS do{ \
	(void) pcl_rmdir(TESTDIR "/a/b/c"); \
	(void) pcl_rmdir(TESTDIR "/a/b"); \
	(void) pcl_rmdir(TESTDIR "/a"); \
	(void) pcl_rmdir(TESTDIR); \
}while(0)

	int r = pcl_mkdirs(TESTDIR "/a/b/c", 0755);

	if(r)
	{
		RMDIRS;
		ASSERT_INTEQ(r, 0, "mkdirs failed");
	}

	struct stat st;
	bool success = stat(TESTDIRA "/a/b/c", &st) == 0 && S_ISDIR(st.st_mode);

	RMDIRS;

	ASSERT_TRUE(success, "missing nested directory for mkdirs");

	return success;
}

/**$ Delete a directory recursively */
TESTCASE(rmdir_r)
{
	ASSERT_INTEQ(pcl_mkdirs(TESTDIR "/a/b/c", 0755), 0, "mkdirs failed");
	ASSERT_INTEQ(pcl_rmdir_r(TESTDIR), 0, "rmdir_r failed");

	struct stat st;
	bool success = stat(TESTDIRA, &st) != 0;

	ASSERT_TRUE(success, "testdir still exists after rmdir_r");

	return success;
}
