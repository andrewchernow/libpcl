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

#include "_sysinfo.h"
#include <pcl/time.h>
#include <pcl/string.h>
#include <pcl/alloc.h>

#ifdef PCL_UNIX
	#include <unistd.h>
	#include <sys/utsname.h>
#else
	#include <Winsock2.h> // gethostname
#endif

void
pcl_sysinfo(pcl_sysinfo_t *info)
{
	char *p;
	pcl_tm_t tm;

#ifdef PCL_UNIX
	struct utsname uts;
	memset(&uts, 0, sizeof(uts));
	(void)uname(&uts);
#else
	int uts;
#endif

	if(!info)
		return;

	memset(info, 0, sizeof(*info));

	/* timezone data */
	if(pcl_localtime(NULL, &tm) == 0)
	{
		info->gmtoff = tm.tm_gmtoff;
		pcl_strcpy(info->tzabbr, sizeof(info->tzabbr), tm.tm_zone);
	}
	else
	{
		strcpy(info->tzabbr, "UTC");
	}

	/* Windows will overwrite the computer name 'info->name' with the
	 * NetBIOS name during the windows_info() call.
	 */
	if(!gethostname(info->hostname, sizeof(info->hostname)))
	{
		if((p = strchr(info->hostname, '.')))
			*p = 0;

		tchar_t *name = pcl_utf8_to_tcs(info->hostname, 0, NULL);
		pcl_tcscpy(info->name, countof(info->name), name);
		pcl_free(name);

		if(p)
			*p = '.';
	}

	ipcl_sysinfo(info, &uts);

	/* remove any double spaces, this is common (straight from DMI) */
	while((p = strstr(info->cpu_model, "  ")))
		memmove(p, p + 1, strlen(p + 1) + 1);

	pcl_strtrim(info->cpu_model);
}
