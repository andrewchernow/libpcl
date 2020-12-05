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

#include <pcl/process.h>
#include <pcl/file.h>
#include <pcl/error.h>
#include <pcl/io.h>
#include <pcl/alloc.h>
#include <ctype.h>

FILE *
pcl_popen(const tchar_t *command, ...)
{
	va_list ap;
	tchar_t *cmd;
	int flags = 0;
	FILE *stream = NULL;

	if(strempty(command))
		return R_SETERRMSG(NULL, PCL_EINVAL, "command must be provided", 0);

	while(isspace(*command))
		command++;

	/* get the mode */
	if(*command == '<')
		flags |= PCL_EXEC_STDOUT;
	else if(*command == '>')
		flags |= PCL_EXEC_STDIN;
	else if(*command == '^') /* like the FISH shell */
		flags |= PCL_EXEC_STDERR;
	else
		return R_SETERRMSG(NULL, PCL_EINVAL, "invalid command mode '%tc'", *command);

	/* optional flags, no spaces between mode and flags! */
	command++;

	if(*command == '!')
	{
		command++;

		if(flags & PCL_EXEC_STDIN)
			flags |= PCL_EXEC_STDIN_NB;
		else if(flags & PCL_EXEC_STDOUT)
			flags |= PCL_EXEC_STDOUT_NB;
		else if(flags & PCL_EXEC_STDERR)
			flags |= PCL_EXEC_STDERR_NB;
	}

	while(isspace(*command))
		command++;

	/* format command */
	va_start(ap, command);
	int r = pcl_vastprintf(&cmd, command, ap);
	va_end(ap);

	if(r < 0)
		return R_SETERR(NULL, -r);

	/* perform an pcl_proc_exec() */
	pcl_proc_exec_t x = {.command = cmd};

	if(pcl_proc_exec(&x, flags))
	{
		pcl_free(cmd);
		return R_TRC(NULL);
	}

	pcl_file_t *f = x.f_stdin;

	if(!f)
		f = x.f_stdout;
	if(!f)
		f = x.f_stderr;

	if(!(stream = pcl_fdopen(f)))
	{
		pcl_file_close(f);
		TRC();
	}

	pcl_free(cmd);

	return stream;
}
