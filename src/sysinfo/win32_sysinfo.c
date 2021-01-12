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

#include <pcl/defs.h>
//#pragma warning (disable : 4100 4711 4127 4702 4706 4100 4514 4710 4201 4206)
#define FORCE_UNICODE
#define INCL_NET
#include <winsock2.h>
#include <ntsecapi.h>
#include <windows.h>
#include <Lm.h>
#include "_sysinfo.h"
#include <pcl/io.h>
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/string.h>

#ifndef SM_SERVERR2
#	define SM_SERVERR2 89
#endif

/* Extract version from registry, preferred method. */
static bool
windows_version(char *buf, int len)
{
	HKEY key;
	int n=0;
	DWORD dwSize;
	char product[80];
	char version[24];
	char build[24];
	char service_pack[80];

	if(RegOpenKeyExA(HKEY_LOCAL_MACHINE,
		"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion",
		0, KEY_READ, &key) != ERROR_SUCCESS)
	{
		return false;
	}

	/* $product v$version (Build $build: $service_pack)
	 * Example:
	 *    Windows Server 2008 (R) Standard v6.0 (Build 6002: Service Pack 2)
	 */

	dwSize = (DWORD)sizeof(product);
	memset(product, 0, dwSize);
	(void)RegQueryValueExA(key, "ProductName", NULL, NULL,
		(LPBYTE)product, &dwSize);

	dwSize = (DWORD)sizeof(version);
	memset(version, 0, dwSize);
	(void)RegQueryValueExA(key, "CurrentVersion", NULL, NULL,
		(LPBYTE)version, &dwSize);

	dwSize = (DWORD)sizeof(build);
	memset(build, 0, dwSize);
	(void)RegQueryValueExA(key, "CurrentBuild", NULL, NULL,
		(LPBYTE)build, &dwSize);

	/* service pack */
	dwSize = (DWORD)sizeof(service_pack);
	memset(service_pack, 0, dwSize);
	(void)RegQueryValueExA(key, "CSDVersion", NULL, NULL,
		(LPBYTE)service_pack, &dwSize);

	RegCloseKey(key);

	if(*product)
	{
		pcl_strcpy(buf, len, product);
		n = (int)strlen(buf);
	}

	if(*version)
	{
		if(n)
			buf[n++] = ' ';

		buf[n++] = 'v';
		pcl_strcpy(buf + n, len - n, version);
		n = (int)strlen(buf);
	}

	if(*build || *service_pack)
	{
		if(n)
			buf[n++] = ' ';

		buf[n++] = '(';

		if(*build)
			n += pcl_sprintf(buf + n, len - n, "Build %s%s",
				build, *service_pack ? ": " : "");

		if(*service_pack)
			n += pcl_sprintf(buf + n, len - n, "%s", service_pack);

		buf[n++] = ')';
		buf[n] = 0;
	}

	return true;
}

static int
windows_langrp_type(const pchar_t *computer_name)
{
	LSA_HANDLE hPolicy = NULL;
	LSA_OBJECT_ATTRIBUTES attrs;
	POLICY_PRIMARY_DOMAIN_INFO *info = NULL;
	int langrp_type = PclLanGrpStandalone;
	POLICY_INFORMATION_CLASS infocls = PolicyPrimaryDomainInformation;
	ACCESS_MASK mask = GENERIC_READ | POLICY_VIEW_LOCAL_INFORMATION;
	LSA_UNICODE_STRING str;

	zero(attrs);

	str.Buffer = (PWSTR)computer_name;
	str.Length = (USHORT)(pcl_pcslen(computer_name) * sizeof(pchar_t));
	str.MaximumLength = str.Length + (1 * sizeof(pchar_t));

	if(!LsaOpenPolicy(&str, &attrs, mask, &hPolicy) &&
		 !LsaQueryInformationPolicy(hPolicy, infocls, &info))
	{
		/* If Sid is present, this computer is on a Domain */
		if(info->Sid)
		{
			langrp_type = PclLanGrpDomain;
		}
		else
		{
			pchar_t domain[4096];

			pcl_pmemcpy(domain, info->Name.Buffer, info->Name.Length);
			domain[info->Name.Length] = 0;

			/* If domain doesn't match computer name, this is a workgroup. */
			if(pcl_pcsnicmp(domain, computer_name, pcl_pcslen(computer_name)))
				langrp_type = PclLanGrpWorkgroup;
		}
	}

	if(info)
		LsaFreeMemory((LPVOID)info);

	if(hPolicy)
		LsaClose(hPolicy);

	return langrp_type;
}

/* Gets the langrp (workgroup/domain) name and the NetBIOS name. */
static int
windows_computer_name(pchar_t *langrp, size_t langrp_size,
	pchar_t *name, size_t name_size, pcl_langrp_t *langrp_type)
{
	LPWKSTA_INFO_100 info100 = NULL;
	pchar_t namebuf[512];

	if(!name)
	{
		name = namebuf;
		name_size = countof(namebuf);
		*name = 0;
	}

	if(langrp)
		*langrp = 0;

	/* Get the Lan Group & Computer name */
	if(NetWkstaGetInfo(NULL, 100, (LPBYTE *)&info100) == NERR_Success)
	{
		if(langrp)
			pcl_pcscpy(langrp, langrp_size, info100->wki100_langroup);

		pcl_pcscpy(name, name_size, info100->wki100_computername);
		NetApiBufferFree(info100);
	}
		/* Try a different method, langrp not provided if workgroup. */
	else
	{
		DWORD len = (DWORD)name_size;

		if(!GetComputerNameEx(ComputerNameNetBIOS, name, &len))
		{
			if(GetLastError() == ERROR_MORE_DATA)
				return SETOSERR(ERROR_MORE_DATA);
		}

		if(langrp)
		{
			len = (DWORD)langrp_size;

			if(!GetComputerNameEx(ComputerNameDnsDomain, langrp, &len))
			{
				if(GetLastError() == ERROR_MORE_DATA)
					return SETOSERR(ERROR_MORE_DATA);
			}
		}
	}

	if(langrp_type && !strempty(name))
		*langrp_type = windows_langrp_type(name);

	return 0;
}

#define PROCNAMESTR "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0"
#define PROCENV "SYSTEM\\CurrentControlSet\\Control\\Session Manager\\Environment"

static void
windows_cpuinfo(pcl_sysinfo_t *info)
{
	HKEY key;
	DWORD dw;
	SYSTEM_LOGICAL_PROCESSOR_INFORMATION *lpilist = NULL;
	DWORD size = (DWORD)sizeof(info->cpu_model);
	LONG l = RegOpenKeyExA(HKEY_LOCAL_MACHINE, PROCNAMESTR, 0, KEY_READ, &key);

	if(l == ERROR_SUCCESS)
	{
		l = RegQueryValueExA(key, "ProcessorNameString", NULL, &dw,
			(LPBYTE)info->cpu_model, &size);

		if(l == ERROR_SUCCESS && size > 1)
			info->cpu_model[size] = 0;

		RegCloseKey(key);
	}

	/* fallback method */
	if(!*info->cpu_model && RegOpenKeyExA(HKEY_LOCAL_MACHINE, PROCENV,
		0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		size = (DWORD)sizeof(info->cpu_model);

		l = RegQueryValueExA(key, "PROCESSOR_IDENTIFIER", NULL, NULL,
			(LPBYTE)info->cpu_model, &size);

		RegCloseKey(key);

		if(l == ERROR_SUCCESS)
			info->cpu_model[size] = 0;
	}

	size = 0;
	if(!GetLogicalProcessorInformation(lpilist, &size) &&
		 GetLastError() == ERROR_INSUFFICIENT_BUFFER)
	{
		int cores = 0, sockets = 0;

		lpilist = pcl_malloc(size);

		if(GetLogicalProcessorInformation(lpilist, &size))
		{
			int i;
			int count = (int)(size / sizeof(SYSTEM_LOGICAL_PROCESSOR_INFORMATION));

			for(i=0; i < count; i++)
			{
				if(lpilist[i].Relationship == RelationProcessorCore)
					cores++;
				else if(lpilist[i].Relationship == RelationProcessorPackage)
					sockets++;
			}
		}

		pcl_free(lpilist);

		if(sockets > 0)
			info->cpu_sockets = sockets;
		if(cores > 0)
			info->cpu_cores = cores;
	}
}

void
ipcl_sysinfo(pcl_sysinfo_t *info, void *_uts)
{
	MEMORYSTATUSEX msex;

	UNUSED(_uts);
	pcl_strcpy(info->vendor, sizeof(info->vendor), "Microsoft");

	/* Get the LAN Group & Computer name */
	windows_computer_name(info->langrp, countof(info->langrp),
		info->name, countof(info->name), &info->langrp_type);

	windows_version(info->version, countof(info->version));
	windows_cpuinfo(info);

	msex.dwLength = (DWORD)sizeof(msex);
	if(GlobalMemoryStatusEx(&msex))
		info->memory = (int64_t)msex.ullTotalPhys;
}

