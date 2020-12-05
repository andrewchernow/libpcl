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

#include <pcl/process.h>
#include <pcl/file.h>
#include <pcl/error.h>
#include <pcl/init.h>
#include <stdlib.h>

int pcl_tmain(int argc, tchar_t **argv)
{
	pcl_proc_exec_t exec = {0};

	pcl_init();

	if(argc < 2)
	{
		fprintf(stderr, "requires one argument, the command to exec: Ex. `exec \"ls -l\"`\n");
		return 1;
	}

	exec.command = argv[1];

	/* For this example, always set PCL_EXEC_SHELL so shell builtin commands work as well as
	 * non-builtins. In a real application, it is advised to omit this flag when it is known
	 * the command to exec is NOT a shell builtin, avoids creating a shell.
	 */
	int flags = PCL_EXEC_STDOUT_NB | PCL_EXEC_STDERR_NB | PCL_EXEC_SHELL;

	if(pcl_proc_exec(&exec, flags))
		PANIC("pcl_proc_exec failed", 0);

	int r;
	char buf[16 * 1024];

	while((r = pcl_file_tryread(exec.f_stdout, buf, sizeof(buf), 2000)))
	{
		if(r == -1)
		{
			if(pcl_errno == PCL_EPIPE)
				break;

			TRC();
			pcl_err_fprintf(stderr, 0, NULL);

			if(pcl_errno != PCL_EAGAIN)
				exit(1);

			printf("\nsleeping...\n");
			pcl_sleep(500, NULL, 0);
		}
		else
		{
			fwrite(buf, 1, r, stdout);
		}
	}

	pcl_file_close(exec.f_stdout);

	while((r = pcl_file_read(exec.f_stderr, buf, sizeof(buf))) > 0)
		fwrite(buf, 1, r, stderr);

	pcl_file_close(exec.f_stderr);

	return 0;
}
