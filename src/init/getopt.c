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

#include "_init.h"
#include <pcl/error.h>
#include <pcl/string.h>
#include <pcl/alloc.h>

int
pcl_getopt(pcl_optstate_t *state, pchar_t **value, pcl_option_t **longopt)
{
	if(!state)
		return BADARG();

	pchar_t *valuebuf;

	if(!value)
		value = &valuebuf;

	*value = NULL;

	/* in the middles of parsing a sequence of short options */
	if(state->next_optstr)
		goto ParseShortOption;

	if(state->argc == state->argidx)
		return 0;

	pchar_t *arg = state->argv[state->argidx];

	/* --------------------------------------------------------------
	 * long option
	 */

	if(pcl_pmemcmp(arg, _P("--"), 2) == 0)
	{
		if(!state->options)
			return SETERRMSG(PCL_EINVAL, "opt[%d] missing options[] for long options '%Ps'",
				state->argidx, arg);

		if(!arg[3])
			return SETERRMSG(PCL_EINVAL, "opt[%d] stray dash found '%Ps'", state->argidx, arg);

		arg += 2;

		pchar_t *eq = pcl_pcschr(arg, _P('='));

		if(eq)
			*eq = 0;

		pcl_option_t *lopt = NULL;

		for(int i = 0; i < state->num_options; i++, lopt = NULL)
		{
			lopt = &state->options[i];
			if(!pcl_pcscmp(lopt->name, arg))
				break;
		}

		if(eq)
			*eq = _P('=');

		if(!lopt)
			return SETERRMSG(PCL_EINVAL, "opt[%d] unknown option '%Ps'", state->argidx, arg);

		if(lopt->policy == PclReqArg)
		{
			/* Form "--path=/home" */
			if(eq)
			{
				if(!eq[1])
					return SETERRMSG(PCL_EINVAL, "opt[%d] missing required argument '%Ps'", state->argidx,
						arg);

				*value = ++eq;
			}
			else if(state->argidx + 1 >= state->argc)
			{
				return SETERRMSG(PCL_EINVAL, "opt[%d] missing required argument '%Ps'", state->argidx, arg);
			}
				/* Form "--path /home" */
			else
			{
				*value = state->argv[++state->argidx];
			}
		}
		else if(lopt->policy == PclOptArg)
		{
			/* Form "--path=/home" */
			if(eq)
			{
				/* ignore empty value for optional */
				if(eq[1])
					*value = eq + 1;
			}
				/* Form "--path /home" */
			else if(state->argidx + 1 < state->argc && *state->argv[state->argidx + 1] != _P('-'))
			{
				*value = state->argv[++state->argidx];
			}
		}

		state->argidx++;

		if(longopt)
			*longopt = lopt;

		return lopt->val;
	}

	if(*arg != _P('-'))
		return SETERRMSG(PCL_EINVAL, "opt[%d] stray characters found '%Ps'", state->argidx, arg);

	/* --------------------------------------------------------------
	 * short option
	 */

	if(!state->optstr)
		return SETERRMSG(PCL_EINVAL, "opt[%d] missing optstr for short options '%Ps'", state->argidx,
			arg);

	if(!arg[1])
		return SETERRMSG(PCL_EINVAL, "opt[%d] stray dash found '%Ps'", state->argidx, arg);

	/* point to short option list: "-l" or "-lpR" w/o dashes */
	state->next_optstr = arg + 1;

ParseShortOption:;

	pchar_t opt = *state->next_optstr++;
	pchar_t *optstr = pcl_pcschr(state->optstr, opt);

	if(!optstr)
		return SETERRMSG(PCL_EINVAL, "opt[%d] unknown option '%Pc'", state->argidx, opt);

	int policy = PclNoArg;
	pchar_t *eq = NULL;

	if(optstr[1] == _P(':'))
	{
		eq = pcl_pcschr(state->next_optstr, _P('='));
		policy = optstr[2] == _P(':') ? PclOptArg : PclReqArg;
	}

	if(policy == PclReqArg)
	{
		/* Form: "-O=value" */
		if(eq)
		{
			if(!eq[1])
				return SETERRMSG(PCL_EINVAL, "opt[%d] missing required argument '%Pc'", state->argidx, opt);

			*value = ++eq;
			state->next_optstr = eq + pcl_pcslen(eq); // point to NUL
		}
			/* Form: "-O value" -- look at next element for value */
		else if(!*state->next_optstr)
		{
			if(state->argidx + 1 >= state->argc)
				return SETERRMSG(PCL_EINVAL, "opt[%d] missing required argument '%Pc'", state->argidx, opt);

			*value = state->argv[++state->argidx];
		}
			/* Form: "-Ovalue" */
		else
		{
			*value = state->next_optstr;
			state->next_optstr += pcl_pcslen(state->next_optstr); // point to NUL
		}
	}
	else if(policy == PclOptArg)
	{
		/* Form: "-O=value" */
		if(eq)
		{
			/* ignore missing optional argument: ie. "-O=" */
			if(eq[1])
				*value = ++eq;

			state->next_optstr = eq + pcl_pcslen(eq); // point to NUL
		}
			/* Form: "-Ovalue" */
		else if(*state->next_optstr)
		{
			*value = state->next_optstr;
			state->next_optstr += pcl_pcslen(state->next_optstr); // point to NUL
		}
			/* Form "-O value" -- In this case, only allow value if it doesn't start with a '-' */
		else if(state->argidx + 1 < state->argc && *state->argv[state->argidx + 1] != _P('-'))
		{
			*value = state->argv[++state->argidx];
		}
	}

	if(!*state->next_optstr)
	{
		state->argidx++;
		state->next_optstr = NULL;
	}

	if(longopt)
		*longopt = NULL;

	/* map 'opt' character to a long option */
	if(longopt && state->options)
	{
		for(int i = 0; i < state->num_options; i++)
		{
			if(state->options[i].val == (int) opt)
			{
				*longopt = &state->options[i];
				break;
			}
		}
	}

	return opt;
}
