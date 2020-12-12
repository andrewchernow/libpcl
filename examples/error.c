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
#include <pcl/error.h>
#include <pcl/strint.h>
#include <pcl/alloc.h>
#include <stdio.h>
#include <string.h>

static FILE *func_three(void)
{
	const char *file = "I-do-not-exists.txt";

	/* fails and sets 'errno' on Unixes and 'errno' or '_doserrno' on Windows */
	FILE *fp = fopen(file, "r");

	/* SETLAST*** macros set the last OS error. */
	if(fp == NULL)
		return R_SETLASTERRMSG(NULL, "failed to open file: '%s'", file);

	return fp;
}

static int func_two(void)
{
	int val;
	char buf[512];
	FILE *fp = func_three();

	if(fp == NULL)
		return TRCMSG("json escapes: \\ \x1f \t \" \b", 0); // add a trace to error set within func_three().

	*buf = 0;
	(void) fgets(buf, sizeof(buf), fp);
	fclose(fp);

	return pcl_strtoi(buf, NULL, 10, &val) == 0 ? val : TRC();
}

static int func_one(void)
{
	if(func_two() < 0)
		return TRCMSG("trace message, providing more context", 0);
	return 0;
}

int main(int argc, char **argv)
{
	UNUSED(argc || argv);

	pcl_init();

	if(func_one() < 0)
	{
		// there is also PTRACE and PANIC macros
		pcl_err_fprintf(stderr, 0, "func_one call failed");

		/* output json version */
		char *json = pcl_err_json("func_one call failed");
		printf("\nJSON Version:\n%s\n", json);
		pcl_free(json);

		return 1;
	}

	return 0;
}
