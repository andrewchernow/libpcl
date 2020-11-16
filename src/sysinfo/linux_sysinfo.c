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

#include "_sysinfo.h"
#include <pcl/defs.h>
#include <pcl/stdio.h>
#include <pcl/string.h>
#include <sys/wait.h>
#include <sys/utsname.h>
#include <stdlib.h>

static void
linux_distro(char *out, size_t len)
{
	int r;
	char buf[512];
	FILE *fp = fopen("/etc/os-release", "r");

	*out = 0;

	/* note: all "modern" distros should have this: /etc/os-release */
	if(fp)
	{
		char name[64];
		char pretty_name[100];
		char version[64];

		*name = *pretty_name = *version = 0;

		while(fgets(buf, sizeof(buf), fp))
		{
			if(!*name && !pcl_strnicmp(buf, "NAME=", 5))
			{
				char *p = pcl_strtrimset(buf + 5, "\"", true, true);
				pcl_strcpy(name, sizeof(name), p);
			}
			else if(!*version && !pcl_strnicmp(buf, "VERSION=", 8))
			{
				char *p = pcl_strtrimset(buf + 8, "\"", true, true);
				pcl_strcpy(version, sizeof(version), p);
			}
			else if(!*pretty_name && !pcl_strnicmp(buf, "PRETTY_NAME=", 12))
			{
				char *p = pcl_strtrimset(buf + 12, "\"", true, true);
				pcl_strcpy(pretty_name, sizeof(pretty_name), p);
			}
		}

		fclose(fp);

		/* Quite a few distros have PRETTY_NAMEs that exclude the codename.
		 * So prefer NAME + VERSION and fallback to PRETTY_NAME or just NAME.
		 * See: https://gist.github.com/natefoo/814c5bf936922dad97ff
		 */
		if(*name && *version)
			pcl_sprintf(out, len, "%s %s", name, version);
		else if(*pretty_name)
			pcl_strcpy(out, len, pretty_name);
		else if(*name)
			pcl_strcpy(out, len, name);

		if(*out)
			return;
	}

	/* first fallback */
	if((fp = popen("lsb_release -d 2>/dev/null", "r")))
	{
		*buf = 0;
		(void) fgets(buf, sizeof(buf), fp);
		r = pclose(fp);

		/* indicates success, check buffer as well */
		if(WIFEXITED(r) && WEXITSTATUS(r) == 0 && pcl_strnicmp(buf, "Description:", 12) == 0)
		{
			/* skip 'Description:' and trim spaces */
			pcl_strcpy(out, len, pcl_strtrim(buf + 12));
			if(*out)
				return;
		}
	}

	/* last ditch effort: older Linux machines: check most common */
	static const char *release_files[] = {
		"/etc/redhat-release",
		"/etc/SuSE-release",
		"/etc/fedora-release",
		"/etc/gentoo-release",
		"/etc/slackware-release",
		"/etc/centos-release",
		"/etc/slackware-version",
	};

	for(*buf = 0, r=0; r < countof(release_files); r++)
	{
		const char *file = release_files[r];

		if((fp = fopen(file, "r")) && fgets(buf, sizeof(buf), fp) && *buf)
			pcl_strcpy(out, len, buf);

		if(fp)
			fclose(fp);

		if(*out)
			return;
	}

	/* upgrade your system */
	pcl_strcpy(out, len, "Linux");
}

static uint64_t
linux_meminfo(void)
{
	uint64_t mem=0;
	char buf[256];
	FILE *fp = fopen("/proc/meminfo", "r");

	if(!fp)
		return mem;

	*buf = 0;
	while(fgets(buf, sizeof(buf), fp))
	{
		if(!pcl_strnicmp(buf, "MemTotal:", 9))
		{
			char *unit;

			/* Example format: '12 kB' */
			mem = strtoll(buf + 9, &unit, 10);
			pcl_strltrim(unit);

			/* appears to always be in kB but play it safe */
			switch(tolower(*unit))
			{
				case 'k': mem *= 1024; break;
				case 'm': mem *= 1048576; break;
				case 'g': mem *= 1073741824; break;
				case 't': mem *= 1099511627776LL; break;
			}

			break;
		}
	}

	fclose(fp);

	return mem;

}

static void
linux_cpuinfo(pcl_sysinfo_t *info)
{
	char buf[1024]; // account for "flags" row
	uint16_t phy_ids[1024]; // limit to 1024 physical CPUs
	FILE *fp = fopen("/proc/cpuinfo", "r");

	if(!fp)
		return;

	while(fgets(buf, sizeof(buf), fp) && info->cpu_sockets < countof(phy_ids))
	{
		if(!pcl_strnicmp(buf, "model name", 10))
		{
			/* note: model could be different across physical CPUs. We don't
			 * handle that unusual case. We assume all are identical.
			 */
			if(!*info->cpu_model)
			{
				char *p = pcl_strtrim(strchr(buf + 10, ':') + 1);
				pcl_strcpy(info->cpu_model, sizeof(info->cpu_model), p);
			}
		}
		else if(!pcl_strnicmp(buf, "physical id", 11))
		{
			int i, id = (int) strtol(strchr(buf + 11, ':') + 1, NULL, 10);

			/* have we seen this CPU (socket) yet? */
			for(i=0; i < info->cpu_sockets; i++)
				if(phy_ids[i] == id)
					break;

			/* new physical CPU socket */
			if(i == info->cpu_sockets)
				phy_ids[info->cpu_sockets++] = id;

			/* hyper-threading or not, add to core count */
			info->cpu_cores++;
		}
	}

	fclose(fp);
}

void
ipcl_sysinfo(pcl_sysinfo_t *info, void *_uts)
{
	struct utsname *uts = _uts;

	linux_distro(info->vendor, sizeof(info->vendor));

	info->memory = linux_meminfo();

	linux_cpuinfo(info);

	pcl_sprintf(info->version, sizeof(info->version), "%s %s %s",
		uts->sysname, uts->release, uts->version);
}


