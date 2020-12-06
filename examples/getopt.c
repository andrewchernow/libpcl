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

int pcl_main(int argc, pchar_t **argv)
{
	pcl_init();

	pcl_option_t options[] = {
		{_P("apple"), PclReqArg, _P('a')},
		{_P("board"), PclOptArg, _P('b')},
		{_P("create"), PclNoArg, _P('c')},
		{_P("delete"), PclNoArg, _P('d')},
		{_P("empty"), PclNoArg, _P('e')}
	};

	pcl_optstate_t *state = pcl_initopt(argc, argv, _P("a:b::cde"), options, countof(options));

	int opt;
	pchar_t *value;
	pcl_option_t *longopt;

	while((opt = pcl_getopt(state, &value, &longopt)) > 0)
	{
		pchar_t *longname = longopt ? longopt->name : _P("<short-only>");

		switch(opt)
		{
			case _P('a'):
				pcl_printf("a:%Ps] %Ps\n", longname, value);
				break;

			case _P('b'):
				pcl_printf("b:%Ps] %Ps\n", longname, value ? value : _P("<no-value>"));
				break;

			case _P('c'):
			case _P('d'):
			case _P('e'):
			default:
				pcl_printf("%Pc:%Ps]\n", (char) opt, longname);
				break;
		}
	}

	pcl_free(state);

	if(opt == -1)
		PANIC("getopt failed", 0);

	return 0;
}
