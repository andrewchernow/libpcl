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

#include <pcl/init.h>
#include <pcl/string.h>
#include <pcl/dir.h>
#include <pcl/error.h>
#include <pcl/stat.h>
#include <pcl/io.h>
#include <pcl/usrgrp.h>
#include <stdlib.h>

int pcl_tmain(int argc, pchar_t **argv)
{
	pcl_init();

	if(argc < 2)
	{
		fprintf(stderr, "Missing required DIR argument\n");
		return 1;
	}

	pcl_dir_t *dp = pcl_opendir(argv[1]);

	if(!dp)
		PANIC(NULL, 0);

	pcl_stat_t st;
	pcl_dirent_t ent;

	/* the 'st' is optional and can be NULL if not needed */
	while(pcl_readdir(dp, &ent, &st))
	{
		pcl_tm_t tm;
		char mode[24], user[80], group[80], fsize[48], time[48];

		pcl_strmode(mode, sizeof(mode), st.mode);
		pcl_strfbytes(fsize, sizeof(fsize), st.size, 1);
		pcl_getuser(st.uid, user, sizeof(user));
		pcl_getgroup(st.gid, group, sizeof(group));
		pcl_localtime(&st.mtime, &tm);
		pcl_strftime(time, sizeof(time), "%b %d %H:%M", &tm);

		pcl_printf("%s %3d %s  %s %8s %s %Ps\n", mode, st.nlink, user, group, fsize, time, ent.name);
	}

	pcl_closedir(dp);

	if(pcl_errno != PCL_ENOMORE)
		PANIC("readdir failed", 0);

	return 0;
}
