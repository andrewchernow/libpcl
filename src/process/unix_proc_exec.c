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

#include "_process.h"
#include "../file/_file.h"
#include <pcl/error.h>
#include <pcl/alloc.h>
#include <pcl/usrgrp.h>
#include <pcl/dir.h>
#include <pcl/limits.h>
#include <pcl/string.h>
#include <pcl/array.h>
#include <stdlib.h> // getenv()

#ifdef PCL_DARWIN
#define LIBPATH "DYLD_LIBRARY_PATH"
#else
#define LIBPATH "LD_LIBRARY_PATH"
#	include <grp.h>
#endif

#define CLOSE_FILES do{ \
  pcl_file_close(exec->f_stdin); \
  pcl_file_close(exec->f_stdout); \
  pcl_file_close(exec->f_stderr); \
  exec->f_stdin = exec->f_stdout = exec->f_stderr = NULL; \
}while(0)

#define CLOSE_FDS do{ \
  if(child_stdin_fd != PCL_BADFD) \
    close(child_stdin_fd); \
  if(child_stdout_fd != PCL_BADFD) \
    close(child_stdout_fd); \
  if(child_stderr_fd != PCL_BADFD) \
    close(child_stderr_fd); \
  child_stdin_fd = child_stderr_fd = child_stdout_fd = PCL_BADFD; \
}while(0)

#define CLOSE_ALL do{ \
  CLOSE_FDS; \
  CLOSE_FILES; \
}while(0)

#define FREEARGV(_argc, _argv) do{ \
  for(int _i = 0; _i < (_argc); _i++) \
    pcl_free_safe((_argv)[_i]);      \
  pcl_free(_argv); \
}while(0)

int
pcl_proc_exec(pcl_proc_exec_t *exec, int flags)
{
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

	pcl_array_t *arr = pcl_proc_parsecmd(pcl_pcsskipws(exec->command));

	if(!arr)
		return TRCMSG("error parsing command: '%Ps'", exec->command);

	pchar_t **argv = (pchar_t **) arr->elements;
	int argc = arr->count;

	/* steal elements */
	arr->count = 0;
	arr->elements = NULL;
	pcl_array_free(arr);

	if(argc == 0)
	{
		FREEARGV(argc, argv);
		return SETERRMSG(PCL_EBADCMD, "command is empty", 0);
	}

	/* use SHELL to execute command */
	if(flags & PCL_EXEC_SHELL)
	{
		pchar_t *shell = getenv("SHELL");

		if(!shell)
			shell = "/bin/sh";

		int tmp_argc = 1;
		pchar_t *tmp_argv[5] = {shell, NULL, NULL, NULL, NULL};

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
		tmp_argv[tmp_argc++] = (pchar_t *) exec->command;

		/* free old argv */
		FREEARGV(argc, argv);

		/* allocate a new argv */
		argc = tmp_argc;
		argv = (pchar_t **) pcl_malloc((argc + 1) * sizeof(pchar_t *));

		/* allocate each argv element */
		for(int i = 0; i < argc; i++)
			argv[i] = pcl_pcsdup(tmp_argv[i]);

		/* must NULL terminate for execvp */
		argv[argc] = NULL;
	}

	pcl_file_t *pipes[2];

	if(flags & (PCL_EXEC_STDIN | PCL_EXEC_STDIN_NB))
	{
		int nonblock = flags & PCL_EXEC_STDIN_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, 0, nonblock | PCL_O_CLOEXEC))
		{
			CLOSE_ALL;
			FREEARGV(argc, argv);
			return SETLASTERR();
		}

		exec->f_stdin = pipes[1];
		child_stdin_fd = pipes[0]->fd;
		pipes[0]->fd = PCL_BADFD;
		pcl_file_close(pipes[0]);
	}

	if(flags & (PCL_EXEC_STDOUT | PCL_EXEC_STDOUT_NB))
	{
		int nonblock = flags & PCL_EXEC_STDOUT_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, nonblock | PCL_O_CLOEXEC, 0))
		{
			CLOSE_ALL;
			FREEARGV(argc, argv);
			return SETLASTERR();
		}

		exec->f_stdout = pipes[0];
		child_stdout_fd = pipes[1]->fd;
		pipes[1]->fd = PCL_BADFD;
		pcl_file_close(pipes[1]);
	}

	if(flags & (PCL_EXEC_STDERR | PCL_EXEC_STDERR_NB))
	{
		int nonblock = flags & PCL_EXEC_STDERR_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, nonblock | PCL_O_CLOEXEC, 0))
		{
			CLOSE_ALL;
			FREEARGV(argc, argv);
			return SETLASTERR();
		}

		exec->f_stderr = pipes[0];
		child_stderr_fd = pipes[1]->fd;
		pipes[1]->fd = PCL_BADFD;
		pcl_file_close(pipes[1]);
	}

	/* additional set of pipes so child process can write an err to parent if execvp fails */
	int err_fds[2];

	if(pipe(err_fds))
	{
		CLOSE_ALL;
		FREEARGV(argc, argv);
		return SETLASTERR();
	}

	exec->pid = fork(); /* Ahhh, finally we can fork */

	/* failed to fork process */
	if(exec->pid < 0)
	{
		FREEZE_ERR
		(
			close(err_fds[0]);
			close(err_fds[1]);
			CLOSE_ALL;
			FREEARGV(argc, argv);
		);

		return SETLASTERR();
	}

	/* parent */
	if(exec->pid > 0)
	{
		err = 0;

		/* close all child_xxx FDs, only for child */
		CLOSE_FDS;

		/* close write end of err FDs, only child uses this */
		close(err_fds[1]);

		/* parent doesn't need this at all, it is for child execvp. Avoid parent leak. */
		FREEARGV(argc, argv);

		/* read err code written when child execvp fails.  Otherwise, this will return zero
		 * (EOF/broken-pipe) since child sets FD_CLOEXEC.
		 */
		if(read(err_fds[0], &err, sizeof(err)) == (ssize_t) sizeof(err))
			CLOSE_FILES; // child failed since this read an err code, cleanup
		else
			exec->start = pcl_time(); /* real start time, not when forked */

		close(err_fds[0]);

		if(err)
			return SETERRMSG(err, "Failed to execute '%Ps'", exec->command);

		if(flags & PCL_EXEC_HANDLE)
			exec->handle = exec->pid;

		return 0;
	}

	/* CHILD .............. */

	/* parent using this, child doesn't need it */
	close(err_fds[0]);

	/* If execvp is successful, err_fds[1] should be automatically closed, causing parent's read
	 * to return 0 (broken pipe)
	 */
	(void) fcntl(err_fds[1], F_SETFD, fcntl(err_fds[1], F_GETFD) | FD_CLOEXEC);

	if(child_stdin_fd != -1)
	{
		pcl_file_close(exec->f_stdin); /* for parent only */
		dup2(child_stdin_fd, STDIN_FILENO);
		close(child_stdin_fd);
	}

	if(child_stdout_fd != -1)
	{
		pcl_file_close(exec->f_stdout); /* for parent only */
		dup2(child_stdout_fd, STDOUT_FILENO);
		close(child_stdout_fd);
	}

	if(child_stderr_fd != -1)
	{
		pcl_file_close(exec->f_stderr); /* for parent only */
		dup2(child_stderr_fd, STDERR_FILENO);
		close(child_stderr_fd);
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

		if(initgroups(exec->user, (gid_t) uid))
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

	if(uid != (uid_t) -1 && setuid(uid))
	{
		TRC();
		pcl_err_fprintf(stderr, 0, "Invalid username for command: "
															 "'%s', user=%s", argv[0], exec->user);
		_exit(1);
	}

	err = PCL_EOKAY;

	/* change working directory if asked to */
	if(!strempty(exec->cwd))
	{
		if(pcl_chdir(exec->cwd) < 0)
			err = pcl_errno;
	}

	/* Set the LDPATH env variable: like LD_LIBRARY_PATH for example. */
	if(err == PCL_EOKAY && (flags & PCL_EXEC_LDPATH))
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

		/* If we don't get an ldpath, ignore the PCL_EXEC_LDPATH request */
		if(ldpath && setenv(LIBPATH, ldpath, 1))
			err = pcl_err_os2pcl(errno);
	}

	/* NOTE: execvp will search system PATH if command doesn't contain a '/'.
	 * Thus, callee should avoid paths with '/' for a shell PATH search.
	 */
	if(err == PCL_EOKAY && execvp(argv[0], (char *const *) argv))
		err = pcl_err_os2pcl(errno);

	/* NEVER GETS HERE IF `execvp` SUCCEEDED !!!
	 *
	 * tell parent there is an error, its waiting on read end of pipe. If execvp succeeded,
	 * err_fds[1] has already been closed via FD_CLOEXEC
	 */
	(void) write(err_fds[1], &err, sizeof(err));

	close(err_fds[1]);
	_exit(1);
}
