/*
  Portable C Library ("PCL")
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

#include <pcl/string.h>
#include <signal.h>

const char *
pcl_strsignal(int signum)
{
#define CASE(name) case name: return #name
	switch(signum)
	{
#ifdef SIGBREAK
		CASE(SIGBREAK);    /* (Windows) Ctrl-Break sequence */
#endif

#ifdef SIGHUP
		CASE(SIGHUP);      /* Hangup (POSIX).  */
#endif

#ifdef SIGINT
		CASE(SIGINT);      /* Interrupt (ANSI).  */
#endif

#ifdef SIGQUIT
		CASE(SIGQUIT);     /* Quit (POSIX).  */
#endif

#ifdef SIGILL
		CASE(SIGILL);      /* Illegal instruction (ANSI).  */
#endif

#ifdef SIGTRAP
		CASE(SIGTRAP);     /* Trace trap (POSIX).  */
#endif

#ifdef SIGABRT
		CASE(SIGABRT);     /* Abort (ANSI).  */
#endif

#ifdef SIGBUS
		CASE(SIGBUS);      /* BUS error (4.2 BSD).  */
#endif

#ifdef SIGFPE
		CASE(SIGFPE);      /* Floating-point exception (ANSI).  */
#endif

#ifdef SIGKILL
		CASE(SIGKILL);     /* Kill, unblockable (POSIX).  */
#endif

#ifdef SIGUSR1
		CASE(SIGUSR1);     /* User-defined signal 1 (POSIX).  */
#endif

#ifdef SIGSEGV
		CASE(SIGSEGV);     /* Segmentation violation (ANSI).  */
#endif

#ifdef SIGUSR2
		CASE(SIGUSR2);     /* User-defined signal 2 (POSIX).  */
#endif

#ifdef SIGPIPE
		CASE(SIGPIPE);     /* Broken pipe (POSIX).  */
#endif

#ifdef SIGALRM
		CASE(SIGALRM);     /* Alarm clock (POSIX).  */
#endif

#ifdef SIGTERM
		CASE(SIGTERM);     /* Termination (ANSI).  */
#endif

#ifdef SIGSTKFLT
		CASE(SIGSTKFLT);   /* Stack fault.  */
#endif

#ifdef SIGCHLD
		CASE(SIGCHLD);     /* Child status has changed (POSIX).  */
#endif

#ifdef SIGCONT
		CASE(SIGCONT);     /* Continue (POSIX).  */
#endif

#ifdef SIGSTOP
		CASE(SIGSTOP);     /* Stop, unblockable (POSIX).  */
#endif

#ifdef SIGTSTP
		CASE(SIGTSTP);     /* Keyboard stop (POSIX).  */
#endif

#ifdef SIGTTIN
		CASE(SIGTTIN);     /* Background read from tty (POSIX).  */
#endif

#ifdef SIGTTOU
		CASE(SIGTTOU);     /* Background write to tty (POSIX).  */
#endif

#ifdef SIGURG
		CASE(SIGURG);      /* Urgent condition on socket (4.2 BSD).  */
#endif

#ifdef SIGXCPU
		CASE(SIGXCPU);     /* CPU limit exceeded (4.2 BSD).  */
#endif

#ifdef SIGXFSZ
		CASE(SIGXFSZ);     /* File size limit exceeded (4.2 BSD).  */
#endif

#ifdef SIGVTALRM
		CASE(SIGVTALRM);   /* Virtual alarm clock (4.2 BSD).  */
#endif

#ifdef SIGPROF
		CASE(SIGPROF);     /* Profiling alarm clock (4.2 BSD).  */
#endif

#ifdef SIGWINCH
		CASE(SIGWINCH);    /* Window size change (4.3 BSD, Sun).  */
#endif

#ifdef SIGIO
		CASE(SIGIO);       /* I/O now possible (4.2 BSD). SIGPOLL sysv */
#endif

#ifdef SIGPWR
		CASE(SIGPWR);      /* Power failure restart (System V).  */
#endif

#ifdef SIGSYS
		CASE(SIGSYS);      /* Bad system call.  */
#endif

		/* all sigs missing, avoid empty switch error ... very unlikely! */
		case -1:;
	}

	return "UNDEF";
}
