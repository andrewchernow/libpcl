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

#include "_json.h"
#include <pcl/strint.h>
#include <stdlib.h>

pcl_json_t *
ipcl_json_parse_number(ipcl_json_state_t *s)
{
	const char *p = s->next;
	bool decimal = false;
	bool exp = false;
	bool expsign = false;

	if(*p == '-')
		p++;

	for(; p < s->end; p++)
	{
		unsigned char c = *p;

		if(c == 'e' || c == 'E')
		{
			if(exp)
				JSON_THROW("invalid number format: multiple exponents '%c'", c);

			exp = true;
		}
		else if(c == '.')
		{
			if(decimal)
				JSON_THROW("invalid number format: multiple decimal signs", 0);

			decimal = true;
		}
		else if(c == '-' || c == '+')
		{
			if(!exp)
				JSON_THROW("invalid number format: sign '%c' in wrong position", c);

			if(expsign)
				JSON_THROW("invalid number format: multiple exponent signs", 0);

			expsign = true;
		}
		/* done, no more digits */
		else if(c < '0' || c > '9')
		{
			break;
		}
	}

	if(p == s->end)
		JSON_THROW("unexpected end of input parsing number", 0);

	if(*s->next == '0' && isdigit(s->next[1]))
		JSON_THROW("invalid number value: begins with 0", 0);

	char *end;
	pcl_json_t *val;

	if(exp || decimal)
	{
		errno = 0;

		double num = strtod(s->next, &end);

		/* ERANGE issues */
		if(errno && num)
		{
			pcl_err_set(PCL_LOCATION_ARGS, pcl_err_crt2pcl(errno), 0, "invalid number value");
			return NULL;
		}

		/* strtod raises an EINVAL. in this case, it means p->next was just "-" or "." */
		if(s->next == end)
			JSON_THROW("invalid number value: no digits found", 0);

		val = pcl_json_real(num);
	}
	else
	{
		long long num;

		if(pcl_strtoll(s->next, &end, 10, &num) < 0)
			return R_TRC(NULL);

		val = pcl_json_integer(num);
	}

	s->next = end;

	return val;
}
