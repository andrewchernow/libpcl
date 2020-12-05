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
#include <pcl/alloc.h>
#include <pcl/error.h>
#include <pcl/io.h>
#include <stdlib.h>

int pcl_tmain(int argc, tchar_t **argv)
{
	pcl_init();

	pcl_option_t options[] = {
		{_T("apple"), PclReqArg, _T('a')},
		{_T("board"), PclOptArg, _T('b')},
		{_T("create"), PclNoArg, _T('c')},
		{_T("delete"), PclNoArg, _T('d')},
		{_T("empty"), PclNoArg, _T('e')}
	};

	pcl_optstate_t *state = pcl_initopt(argc, argv, _T("a:b::cde"), options, countof(options));

	int opt;
	tchar_t *value;
	pcl_option_t *longopt;

	while((opt = pcl_getopt(state, &value, &longopt)) > 0)
	{
		tchar_t *longname = longopt ? longopt->name : _T("<short-only>");

		switch(opt)
		{
			case _T('a'):
				pcl_printf("a:%ts] %ts\n", longname, value);
				break;

			case _T('b'):
				pcl_printf("b:%ts] %ts\n", longname, value ? value : _T("<no-value>"));
				break;

			case _T('c'):
			case _T('d'):
			case _T('e'):
			default:
				pcl_printf("%tc:%ts]\n", (char) opt, longname);
				break;
		}
	}

	pcl_free(state);

	if(opt == -1)
		PANIC("getopt failed", 0);

	return 0;
}
