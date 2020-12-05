
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

#ifndef LIBPCL_SYSINFO_H
#define LIBPCL_SYSINFO_H

#include <pcl/limits.h>
#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* only useful for windows: pcl_sysinfo_t.langrp_type */
typedef enum
{
	PclLanGrpStandalone, // unixes always get this value
	PclLanGrpWorkgroup,
	PclLanGrpDomain
} pcl_langrp_t;

typedef struct
{
	int cpu_sockets;      /* number of physical processors (sockets) */
	int cpu_cores;        /* number of logical processors (cores/threads) */
	char cpu_model[100];  /* Intel(R) Core(TM) i7-3720QM CPU @ 2.60GHz */
	long gmtoff;
	char tzabbr[80];
	char hostname[PCL_MAXDOMAINLEN];
	tchar_t name[64];     // computer name
	uint64_t memory;      // bytes
	char version[100];    /* Mac OS X Catalina 10.15.7/19.6.0 (Build 19H2) */
	char vendor[64];      /* Apple, Microsoft, Ubuntu 18.04 LTS (Bionic Beaver) */

	/* windows only */
	tchar_t langrp[64];
	pcl_langrp_t langrp_type;
} pcl_sysinfo_t;

PCL_EXPORT void pcl_sysinfo(pcl_sysinfo_t *info);

#ifdef __cplusplus
}
#endif
#endif

