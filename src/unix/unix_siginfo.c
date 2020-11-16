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

#include <pcl/stdio.h>
#include <pcl/string.h>
#include <signal.h>

void
pcl_unix_siginfo(char *buf, size_t bufl, siginfo_t *info)
{
	int l = 0;
	int n;
	char *reason = "unknown";

	if(!info)
	{
		if(buf && bufl > 0)
			*buf = 0;
		return;
	}

	char code[64]={0};

	if(info->si_code > 0)
		pcl_sprintf(code, sizeof(code), " code=%d", info->si_code);

	l = pcl_sprintf(buf, bufl, "%s%s", pcl_strsignal(info->si_signo), code);
	if(l < 0)
		return;

	/* append reason, si_value and si_addr if signal fills it in */
	switch(info->si_signo)
	{
		case SIGILL:
		{
			switch(info->si_code)
			{
#ifdef ILL_ILLOPC
				case ILL_ILLOPC:
					reason = "Illegal opcode";
					break;
#endif

#ifdef ILL_ILLOPN
				case ILL_ILLOPN:
					reason = "Illegal operand";
					break;
#endif

#ifdef ILL_ILLADR
				case ILL_ILLADR:
					reason = "Illegal addressing mode";
					break;
#endif

#ifdef ILL_ILLTRP
				case ILL_ILLTRP:
					reason = "Illegal trap";
					break;
#endif

#ifdef ILL_PRVOPC
				case ILL_PRVOPC:
					reason = "Privileged opcode";
					break;
#endif

#ifdef ILL_PRVREG
				case ILL_PRVREG:
					reason = "Privileged register";
					break;
#endif

#ifdef ILL_COPROC
				case ILL_COPROC:
					reason = "Coprocessor error";
					break;
#endif

#ifdef ILL_BADSTK
				case ILL_BADSTK:
					reason = "Internal stack error";
					break;
#endif

				default:
					break;
			}

			goto PRINTERROR;
		}

		case SIGFPE:
		{
			switch(info->si_code)
			{
				case FPE_INTDIV:
					reason = "Integer divide-by-zero";
					break;

				case FPE_INTOVF:
					reason = "Integer overflow";
					break;

				case FPE_FLTDIV:
					reason = "Floating point divide-by-zero";
					break;

				case FPE_FLTOVF:
					reason = "Floating point overflow";
					break;

				case FPE_FLTUND:
					reason = "Floating point underflow";
					break;

				case FPE_FLTRES:
					reason = "Floating point inexact result";
					break;

				case FPE_FLTINV:
					reason = "Invalid floating point operation";
					break;

				case FPE_FLTSUB:
					reason = "Subscript out of range";
					break;
			}

			goto PRINTERROR;
		}

		case SIGSEGV:
		{
#ifdef SEGV_MAPERR
			if(info->si_code == SEGV_MAPERR)
			{
				reason = "Address not mapped";
				goto PRINTERROR;
			}
#endif

#ifdef SEGV_ACCERR
			if(info->si_code == SEGV_ACCERR)
				reason = "Invalid permissions";
#endif
			goto PRINTERROR;
		}

		case SIGBUS:
		{
#ifdef BUS_ADRALN
			if(info->si_code == BUS_ADRALN)
			{
				reason = "Invalid address alignment";
				goto PRINTERROR;
			}
#endif

#ifdef BUS_ADRERR
			if(info->si_code == BUS_ADRERR)
			{
				reason = "Non-existent physical address";
				goto PRINTERROR;
			}
#endif

#ifdef BUS_OBJERR
			if(info->si_code == BUS_OBJERR)
				reason = "Object-specific hardware error";
#endif

PRINTERROR:
			n = pcl_sprintf(buf+l, bufl-l, ": reason=%s, addr=0x%lx",
				reason, info->si_addr);

			if(n < 0)
				return;

			l += n;
			break;
		}

		case SIGCHLD:
		{
			char tmp[32];
			*tmp = 0;
			switch(info->si_code)
			{
#ifdef CLD_EXITED
				case CLD_EXITED:
					reason = "exited";
					sprintf(tmp, ", exitcode=%d", info->si_status);
					break;
#endif

#ifdef CLD_KILLED
				case CLD_KILLED:
					reason = "killed";
					sprintf(tmp, ", chldsig=%s",
						pcl_strsignal(info->si_value.sival_int));
					break;
#endif

#ifdef CLD_DUMPED
				case CLD_DUMPED:
					reason = "dumped";
					sprintf(tmp, ", chldsig=%s",
						pcl_strsignal(info->si_value.sival_int));
					break;
#endif

#ifdef CLD_TRAPPED
				case CLD_TRAPPED:
					reason = "trapped";
					break;
#endif

#ifdef CLD_STOPPED
				case CLD_STOPPED:
					reason = "stopped";
					break;
#endif

#ifdef CLD_CONTINUED
				case CLD_CONTINUED:
					reason = "continued";
					break;
#endif

				default:
					break;
			}

			if((n = pcl_sprintf(buf+l, bufl-l, ": reason=%s%s", reason, tmp)) < 0)
				return;

			l += n;
			break;
		}
	}

	/* only append errors, instead of "0 Success" everywhere */
	if(info->si_errno)
		pcl_sprintf(buf+l, bufl-l, ", errno=%d, errmsg=%s", info->si_errno, strerror(info->si_errno));
}


