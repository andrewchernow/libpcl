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

#include "_process.h"
#include "../file/_file.h"
#include <pcl/error.h>
#include <pcl/stdlib.h>
#include <pcl/tchar.h>
#include <pcl/usrgrp.h>
#include <pcl/dir.h>
#include <pcl/limits.h>

#ifdef PCL_DARWIN
	#define LIBPATH "DYLD_LIBRARY_PATH"
#else
	#define LIBPATH "LD_LIBRARY_PATH"
#endif

#define CLOSE_FILES do{ \
	if(exec->f_stdin) \
		pcl_close(exec->f_stdin); \
	if(exec->f_stdout) \
		pcl_close(exec->f_stdout); \
	if(exec->f_stderr) \
		pcl_close(exec->f_stderr); \
	exec->f_stdin = exec->f_stdout = exec->f_stderr = NULL;\
}while(0)

#define CLOSE_FDS do{ \
	if(child_stdin_fd != -1) \
		close(child_stdin_fd); \
	if(child_stdout_fd != -1) \
		close(child_stdout_fd); \
	if(child_stderr_fd != -1) \
		close(child_stderr_fd); \
	child_stdin_fd = child_stderr_fd = child_stdout_fd = -1; \
}while(0)

#define CLOSE_ALL do{ \
	CLOSE_FDS; \
	CLOSE_FILES; \
}while(0)

static int
create_stdio_pipes(int flags, int *rd, int *wr)
{
	int fds[2];

	if(pipe(fds))
		return SETLASTERR();

	if(flags & PCL_PREX_NONBLOCK)
	{
		(void)fcntl(fds[0], F_SETFL, fcntl(fds[0], F_GETFL, 0) | O_NONBLOCK);
		(void)fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL, 0) | O_APPEND | O_NONBLOCK);
	}
	else
	{
		(void)fcntl(fds[1], F_SETFL, fcntl(fds[1], F_GETFL, 0) | O_APPEND);
	}

	if(flags & PCL_PREX_CLOEXEC)
		(void)fcntl(fds[1], F_SETFD, FD_CLOEXEC);

	*rd = fds[0];
	*wr = fds[1];
	return 0;
}

int
pcl_proc_exec(pcl_proc_exec_t *exec, int flags)
{
	int fds[2];
	int err;
	int child_stdin_fd = -1, child_stdout_fd = -1, child_stderr_fd = -1;

	exec->pid = 0;
	exec->handle = 0;
	exec->start.sec = 0;
	exec->start.nsec = 0;
	exec->f_stderr = exec->f_stdin = exec->f_stdout = NULL;

	if(strempty(exec->command))
		return BADARG();

	fflush(stdout);
	fflush(stderr);

	tchar_t **argv = NULL;
	int argc = pcl_proc_parsecmd(pcl_tskipblanks(exec->command), &argv);

	if(argc == 0)
	{
		pcl_proc_freeargv(argc, argv);
		return SETERRMSG(PCL_EBADCMD, "command is empty", 0);
	}

	if(argc < 0)
		return TRCMSG("error parsing command", 0);

	/* use SHELL to execute command */
	if(flags & PCL_PREX_SHELL)
	{
		tchar_t *shell = getenv("SHELL");

		if(!shell)
			shell = "/bin/sh";

		int tmp_argc = 1;
		tchar_t *tmp_argv[5] = {shell, NULL, NULL, NULL, NULL};

		/* set -m for tcsh/csh: loads ~/.tcshrc */
		if(pcl_stristr(shell, "/tcsh") || pcl_stristr(shell, "/csh"))
			tmp_argv[tmp_argc++] = "-m";
			/* note: fish, dash, sh and bash will all set -l, login shell */
		else if(!pcl_stristr(shell, "/zsh"))
			tmp_argv[tmp_argc++] = "-l";

		/* Man Page: "commands are read from this argument" ... (must be a single
		 * arg, at shell this means quoted). Ex: `bash -l -c "printf '%s\n' 'Hello'"`,
		 * is how pcl_proc_exec() would send this to the shell for builtin `printf`.
		 */
		tmp_argv[tmp_argc++] = "-c";
		tmp_argv[tmp_argc++] = (tchar_t*)exec->command;

		/* free old argv */
		pcl_proc_freeargv(argc, argv);

		/* allocate a new argv */
		argc = tmp_argc;
		argv = (tchar_t **)pcl_malloc((argc + 1) * sizeof(tchar_t *));

		/* allocate each argv element */
		for(int i=0; i < argc; i++)
			argv[i] = pcl_tcsdup(tmp_argv[i]);

		/* must NULL terminate for execvp */
		argv[argc] = NULL;
	}

	int fd, stdflags = (flags & PCL_PREX_NONBLOCK) ? PCL_FF_NONBLOCK : 0;

	if(flags & PCL_PREX_STDIN)
	{
		if(create_stdio_pipes(flags, &child_stdin_fd, &fd))
		{
			CLOSE_ALL;
			pcl_proc_freeargv(argc, argv);
			return SETLASTERR();
		}

		exec->f_stdin = ipcl_file_alloc(fd, stdflags | PCL_FF_APPEND | PCL_FF_WRITE);
	}

	if(flags & PCL_PREX_STDERR)
	{
		if(create_stdio_pipes(flags, &fd, &child_stderr_fd))
		{
			CLOSE_ALL;
			pcl_proc_freeargv(argc, argv);
			return SETLASTERR();
		}

		exec->f_stderr = ipcl_file_alloc(fd, stdflags | PCL_FF_READ);
	}

	if(flags & PCL_PREX_STDOUT)
	{
		if(create_stdio_pipes(flags, &fd, &child_stdout_fd))
		{
			CLOSE_ALL;
			pcl_proc_freeargv(argc, argv);
			return SETLASTERR();
		}

		exec->f_stdout = ipcl_file_alloc(fd, stdflags | PCL_FF_READ);
	}

	if(pipe(fds))
	{
		CLOSE_ALL;
		pcl_proc_freeargv(argc, argv);
		return SETLASTERR();
	}

	exec->pid = fork(); /* Ahhh, finally we can fork */

	/* failed to fork process */
	if(exec->pid < 0)
	{
		close(fds[0]);
		close(fds[1]);
		CLOSE_ALL;
		pcl_proc_freeargv(argc, argv);
		return SETLASTERR();
	}

	/* parent */
	if(exec->pid > 0)
	{
		err = 0;
		CLOSE_FDS;
		close(fds[1]);
		pcl_proc_freeargv(argc, argv); /* only for child */

		/* read err code written when execvp fails.  Otherwise, this will
		 * return zero (EOF/broken-pipe) since child sets FD_CLOEXEC.
		 */
		if(read(fds[0], &err, sizeof(err)) == (ssize_t)sizeof(err))
			CLOSE_FILES;
		else
			exec->start = pcl_time(); /* real start time, not when forked */

		close(fds[0]);

		if(err)
			return SETERRMSG(err, "Failed to execute '%ts'", exec->command);

		return 0;
	}

	/* CHILD .............. */

	/* If execvp works, fds[1] should be closed, causing parent's
	 * read to return 0 (broken pipe)
	 */
	(void)fcntl(fds[1], F_SETFD, FD_CLOEXEC);

	/* parent using this, child doesn't need it */
	close(fds[0]);

	if(child_stdin_fd != -1)
	{
		pcl_close(exec->f_stdin); /* for parent only */
		dup2(child_stdin_fd, STDIN_FILENO);
		close(child_stdin_fd);
	}

	if(child_stderr_fd != -1)
	{
		pcl_close(exec->f_stderr); /* for parent only */
		dup2(child_stderr_fd, STDERR_FILENO);
		close(child_stderr_fd);
	}

	if(child_stdout_fd != -1)
	{
		pcl_close(exec->f_stdout); /* for parent only */
		dup2(child_stdout_fd, STDOUT_FILENO);
		close(child_stdout_fd);
	}

	/* The below code impersonates the user and group requested by callee.
	 * Not the effective user/group, the real ones.
	 */
	uid_t uid = -1;

	/* If requested username, ensure the group list is correct */
	if(exec->user)
	{
		if(pcl_getuid(exec->user, &uid))
		{
			TRC();
			pcl_err_fprintf(stderr, 0, "Invalid username for command: "
																 "'%s', user=%s", argv[0], exec->user);
			_exit(1);
		}

		if(initgroups(exec->user, (gid_t)uid))
		{
			TRC();
			pcl_err_fprintf(stderr, 0, "Cannot init group access list for command: "
																 "'%s', user=%s", argv[0], exec->user);
			_exit(1);
		}
	}

	if(exec->group)
	{
		gid_t gid;

		if(pcl_getgid(exec->group, &gid))
		{
			TRC();
			pcl_err_fprintf(stderr, 0, "Invalid group for command: "
																 "'%s', group=%s", argv[0], exec->group);
			_exit(1);
		}

		if(setgid(gid))
		{
			TRC();
			pcl_err_fprintf(stderr, 0, "Invalid group for command: "
																 "'%s', group=%s", argv[0], exec->group);
			_exit(1);
		}
	}

	if(uid != (uid_t)-1 && setuid(uid))
	{
		TRC();
		pcl_err_fprintf(stderr, 0, "Invalid username for command: "
															 "'%s', user=%s", argv[0], exec->user);
		_exit(1);
	}

	err = 0;

	/* change working directory if asked to */
	if(!strempty(exec->cwd) && pcl_chdir(exec->cwd))
	{
		err = pcl_errno;
	}
		/* Set the LDPATH env variable: like LD_LIBRARY_PATH for example. */
	else if(flags & PCL_PREX_LDPATH)
	{
		const char *ldpath = exec->ldpath;
		char cwd[PCL_MAXPATH];

		/* try fallbacks, the exec->cwd fallback just helps us avoid a call
		 * to getcwd when we already have the cwd.
		 */
		if(!ldpath)
		{
			ldpath = exec->cwd;
			if(!ldpath)
				ldpath = getcwd(cwd, countof(cwd));
		}

		/* If we don't get an ldpath, ignore the PCL_PREX_LDPATH request */
		if(ldpath && setenv(LIBPATH, ldpath, 1))
			err = pcl_err_os2pcl(errno);
	}

	/* NOTE: execvp will search system PATH if command doesn't contain a '/'.
	 * Thus, callee should avoid paths with '/' for a shell PATH search.
	 */
	if(err == 0 && execvp(argv[0], (char *const *)argv))
		err = pcl_err_os2pcl(errno);

	/* NEVER GETS HERE IF `execvp` SUCCEEDED !!!
	 *
	 * tell parent there is an error, its waiting on read end of pipe
	 */
	if(write(fds[1], &err, sizeof(err)) < 0)
		(void)0;//do what exactly? this just quiets GCC, who cares since execvp failed

	close(fds[1]);
	_exit(1);
}
