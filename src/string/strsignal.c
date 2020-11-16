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

#include <signal.h>

const char *
pcl_strsignal(int signum)
{
#define _case(name) case name: return #name
	switch(signum)
	{
#ifdef SIGBREAK
		_case(SIGBREAK);    /* (Windows) Ctrl-Break sequence */
#endif

#ifdef SIGHUP
		_case(SIGHUP);      /* Hangup (POSIX).  */
#endif

#ifdef SIGINT
		_case(SIGINT);      /* Interrupt (ANSI).  */
#endif

#ifdef SIGQUIT
		_case(SIGQUIT);     /* Quit (POSIX).  */
#endif

#ifdef SIGILL
		_case(SIGILL);      /* Illegal instruction (ANSI).  */
#endif

#ifdef SIGTRAP
		_case(SIGTRAP);     /* Trace trap (POSIX).  */
#endif

#ifdef SIGABRT
		_case(SIGABRT);     /* Abort (ANSI).  */
#endif

#ifdef SIGBUS
		_case(SIGBUS);      /* BUS error (4.2 BSD).  */
#endif

#ifdef SIGFPE
		_case(SIGFPE);      /* Floating-point exception (ANSI).  */
#endif

#ifdef SIGKILL
		_case(SIGKILL);     /* Kill, unblockable (POSIX).  */
#endif

#ifdef SIGUSR1
		_case(SIGUSR1);     /* User-defined signal 1 (POSIX).  */
#endif

#ifdef SIGSEGV
		_case(SIGSEGV);     /* Segmentation violation (ANSI).  */
#endif

#ifdef SIGUSR2
		_case(SIGUSR2);     /* User-defined signal 2 (POSIX).  */
#endif

#ifdef SIGPIPE
		_case(SIGPIPE);     /* Broken pipe (POSIX).  */
#endif

#ifdef SIGALRM
		_case(SIGALRM);     /* Alarm clock (POSIX).  */
#endif

#ifdef SIGTERM
		_case(SIGTERM);     /* Termination (ANSI).  */
#endif

#ifdef SIGSTKFLT
		_case(SIGSTKFLT);   /* Stack fault.  */
#endif

#ifdef SIGCHLD
		_case(SIGCHLD);     /* Child status has changed (POSIX).  */
#endif

#ifdef SIGCONT
		_case(SIGCONT);     /* Continue (POSIX).  */
#endif

#ifdef SIGSTOP
		_case(SIGSTOP);     /* Stop, unblockable (POSIX).  */
#endif

#ifdef SIGTSTP
		_case(SIGTSTP);     /* Keyboard stop (POSIX).  */
#endif

#ifdef SIGTTIN
		_case(SIGTTIN);     /* Background read from tty (POSIX).  */
#endif

#ifdef SIGTTOU
		_case(SIGTTOU);     /* Background write to tty (POSIX).  */
#endif

#ifdef SIGURG
		_case(SIGURG);      /* Urgent condition on socket (4.2 BSD).  */
#endif

#ifdef SIGXCPU
		_case(SIGXCPU);     /* CPU limit exceeded (4.2 BSD).  */
#endif

#ifdef SIGXFSZ
		_case(SIGXFSZ);     /* File size limit exceeded (4.2 BSD).  */
#endif

#ifdef SIGVTALRM
		_case(SIGVTALRM);   /* Virtual alarm clock (4.2 BSD).  */
#endif

#ifdef SIGPROF
		_case(SIGPROF);     /* Profiling alarm clock (4.2 BSD).  */
#endif

#ifdef SIGWINCH
		_case(SIGWINCH);    /* Window size change (4.3 BSD, Sun).  */
#endif

#ifdef SIGIO
		_case(SIGIO);       /* I/O now possible (4.2 BSD). SIGPOLL sysv */
#endif

#ifdef SIGPWR
		_case(SIGPWR);      /* Power failure restart (System V).  */
#endif

#ifdef SIGSYS
		_case(SIGSYS);      /* Bad system call.  */
#endif

		/* all sigs missing, avoid empty switch error ... very unlikely! */
		case -1:;
	}

	return "UNDEF";
}
