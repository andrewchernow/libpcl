/*
  Portable C Library (PCL)
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

#include <pcl/init.h>
#include <pcl/sysinfo.h>
#include <pcl/io.h>
#include <pcl/defs.h>
#include <pcl/string.h>

int main(int argc, char **argv)
{
	pcl_sysinfo_t info;

	UNUSED(argc || argv);
	pcl_init();

	pcl_sysinfo(&info);

	char mem[48];

	pcl_strfbytes(mem, sizeof(mem), info.memory, 1);
	pcl_printf(
		"cpu_sockets: %d\n"
		"cpu_cores:   %d\n"
		"cpu_model:   %s\n"
		"gmtoff:      %ld\n"
		"tzabbr:      %s\n"
		"hostname:    %s\n"
		"name:        %Ps\n"
		"memory:      %s (%llu)\n"
		"version:     %s\n"
		"vendor:      %s\n",
		info.cpu_sockets,
		info.cpu_cores,
		info.cpu_model,
		info.gmtoff,
		info.tzabbr,
		info.hostname,
		info.name,
		mem, info.memory,
		info.version,
		info.vendor
	);

	return 0;
}
