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

#include "_error.h"
#include <pcl/buf.h>
#include <pcl/strint.h>
#include <string.h>

int
pcl_err_setjson(const char *json)
{
	pcl_err_t *err = pcl_err_get();

	if(err->frozen)
		return 0;

	pcl_err_clear();

	pcl_buf_t b;

	pcl_buf_init(&b, 512, PclBufText);

	for(const char *j = json; *j; )
	{
		if(*j != '\\')
		{
			pcl_buf_putchar(&b, *j++);
			continue;
		}

		switch(*++j)
		{
			case '\0':
				break;

			case '\\':
			case '"':
			case '/':
				pcl_buf_putchar(&b, *j++);
				break;

			case 'b':
				j++;
				pcl_buf_putstr(&b, "\b");
				break;

			case 't':
				j++;
				pcl_buf_putstr(&b, "\t");
				break;

			case 'n':
				j++;
				pcl_buf_putstr(&b, "\n");
				break;

			case 'f':
				j++;
				pcl_buf_putstr(&b, "\f");
				break;

			case 'r':
				j++;
				pcl_buf_putstr(&b, "\r");
				break;

			case 'u':
			{
				const char *s = j + 1;

				if(isxdigit(*s) && isxdigit(s[1]) && isxdigit(s[2]) && isxdigit(s[3]))
				{
					int val;
					char buf[5];

					memcpy(buf, s, 4);
					buf[4] = 0;

					if(pcl_strtoi(s, NULL, 16, &val) == 0 && val < 0x20)
					{
						pcl_buf_putf(&b, "%c", (char) val);
						j += 5; // skip u0000
						break;
					}
				}

				// fall-through
			}

			default:
				pcl_buf_putf(&b, "\\%c", *j++);
		}
	}

	return 0;
}
