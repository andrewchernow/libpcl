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

#include "_sysinfo.h"
#include <pcl/string.h>
#include <sys/sysctl.h>
#include <sys/utsname.h>
#include <stdio.h>
#include <stdlib.h>

#define PRODNAME "productname:"
#define PRODVER  "productversion:"
#define BUILDVER "buildversion:"

void
ipcl_sysinfo(pcl_sysinfo_t *info, void *_uts)
{
	size_t n;
	FILE *fp;
	char buf[4096];
	const char *codename = "";
	struct utsname *uts = _uts;
	char prodname[80], prodver[40], prodbuild[40];

	pcl_strcpy(info->vendor, sizeof(info->vendor), "Apple Inc.");

	/* This is the only way to get Marketing Version (10.8.3).  It also outputs product name and
	 * product build so we might as well capture them, eventhough there are other ways.  'sw_vers'
	 * has been available since 10.4.
	 */
	if((fp = popen("sw_vers", "r")))
	{
		while(fgets(buf, sizeof(buf), fp))
		{
			/* Product Name: Mac OS X, macOS, macOS Server, etc... */
			if(!pcl_strnicmp(buf, PRODNAME, sizeof(PRODNAME) - 1))
				strcpy(prodname, pcl_strtrim(buf + (sizeof(PRODNAME) - 1)));
				/* Product (marketing) version: 10.8.3 */
			else if(!pcl_strnicmp(buf, PRODVER, sizeof(PRODVER) - 1))
				strcpy(prodver, pcl_strtrim(buf + (sizeof(PRODVER) - 1)));
				/* OS Build: 12D78 */
			else if(!pcl_strnicmp(buf, BUILDVER, sizeof(BUILDVER) - 1))
				strcpy(prodbuild, pcl_strtrim(buf + (sizeof(BUILDVER) - 1)));
		}

		pclose(fp);
	}

	char verbuf[20], *p;

	strcpy(verbuf, prodver);

	p = strrchr(verbuf, '.');
	memmove(p, p + 1, strlen(p + 1) + 1);

	double ver = strtod(verbuf, NULL);

	/* Leopard was Oct 2007, no need to go back any further */
	if(ver >= 10.5 && ver < 10.6)
		codename = "Leopard ";
	else if(ver >= 10.6 && ver < 10.7)
		codename = "Snow Leopard ";
	else if(ver >= 10.7 && ver < 10.8)
		codename = "Lion ";
	else if(ver >= 10.8 && ver < 10.9)
		codename = "Mountain Lion ";
	else if(ver >= 10.9 && ver < 10.10)
		codename = "Mavericks ";
	else if(ver >= 10.10 && ver < 10.11)
		codename = "Yosemite ";
	else if(ver >= 10.11 && ver < 10.12)
		codename = "El Capitan ";
	else if(ver >= 10.12 && ver < 10.13)
		codename = "Sierra ";
	else if(ver >= 10.13 && ver < 10.14)
		codename = "High Sierra ";
	else if(ver >= 10.14 && ver < 10.15)
		codename = "Mojave ";
	else if(ver >= 10.15 && ver < 10.16)
		codename = "Catalina ";

	/* Example: Mac OS X Mountain Lion 10.8.3/12.3.0 (Build 12D78) */
	sprintf(info->version, "%s %s%s/%s (Build %s)", prodname, codename,
		prodver, uts->release, prodbuild);

	/* total physical memory on system */
	n = sizeof(info->memory);
	(void) sysctlbyname("hw.memsize", &info->memory, &n, NULL, 0);

	/* number of processor sockets (physical CPUs) */
	n = 4;
	(void) sysctlbyname("hw.packages", &info->cpu_sockets, &n, NULL, 0);

	/* number of processor cores/threads */
	n = 4;
	(void) sysctlbyname("hw.logicalcpu", &info->cpu_cores, &n, NULL, 0);

	/* Processor Brand: Intel(R) Core(TM) i7-3820QM CPU @ 2.70GHz */
	*buf = 0;
	n = sizeof(info->cpu_model);
	(void) sysctlbyname("machdep.cpu.brand_string", info->cpu_model, &n, NULL, 0);
}

