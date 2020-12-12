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

#include "_process.h"
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/string.h>

int
pcl_proc_parsecmd(const pchar_t *shell_cmd, pchar_t ***out)
{
	const pchar_t *start;
	bool in_quote = false;
	pchar_t **argv;
	size_t size = 4;
	int count = 0;

	if(!shell_cmd)
		return BADARG();

	shell_cmd = pcl_pcsskipws(shell_cmd);
	if(*shell_cmd == '"')
	{
		in_quote = true;
		shell_cmd++;
	}

	start = shell_cmd;
	argv = (pchar_t **)pcl_malloc((size + 1) * sizeof(pchar_t *));

	while(true)
	{
		switch(*shell_cmd)
		{
			case '\\':
				shell_cmd += 2;
				break;

			case '\0':
			case '"':
			case ' ':
			case '\t':
			{
				size_t n;

				if(*shell_cmd == '"')
				{
					if(!in_quote)
					{
						pcl_proc_freeargv(count, argv);
						return SETERRMSG(PCL_ESYNTAX, "unescaped quote", 0);
					}

					in_quote = false;
				}
				else if(in_quote)
				{
					if(*shell_cmd == '\0')
					{
						pcl_proc_freeargv(count, argv);
						return SETERRMSG(PCL_ESYNTAX, "unterminated quote", 0);
					}

					shell_cmd++;
					break;
				}

				n = shell_cmd - start;

				if(n > 0)
				{
					if(count == (int)size)
					{
						size = (size * 3) / 2;
						argv = (pchar_t **)pcl_realloc(argv, (size + 1) * sizeof(pchar_t *));
					}

					argv[count++] = pcl_pcsndup(start, shell_cmd - start);
				}

				if(*shell_cmd == '\0')
					goto finish;

				shell_cmd = pcl_pcsskipws(++shell_cmd);

				if(*shell_cmd == '"')
				{
					in_quote = true;
					shell_cmd++;
				}

				start = shell_cmd;
				break;
			}

			default:
				shell_cmd++;
				break;
		}
	}

finish:

	argv[count] = NULL;
	if(out)
		*out = argv;

	return count;
}



