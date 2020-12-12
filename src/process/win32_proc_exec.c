/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBFILITY OF SUCH DAMAGE.
*/

#include "_process.h"
#include "../win32/_win32.h"
#include "../file/_file.h"
#include <pcl/string.h>
#include <pcl/io.h>
#include <pcl/alloc.h>
#include <userenv.h>
#include <lm.h>

static bool createprocess(pcl_proc_exec_t *exec, const pchar_t *comspec,
	pchar_t *command, STARTUPINFO *si, PROCESS_INFORMATION *pi);

int
pcl_proc_exec(pcl_proc_exec_t *exec, int flags)
{
	bool success = false;
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	SECURITY_ATTRIBUTES sa;
	pcl_file_t *pipes[2];

	if(!exec || strempty(exec->command))
		return BADARG();

	/* zero output params */
	exec->pid = 0;
	zero(exec->start);
	exec->handle = 0;
	exec->f_stderr = exec->f_stdin = exec->f_stdout = NULL;

	zero(sa);
	sa.nLength = sizeof(sa);
	sa.bInheritHandle = true;
	sa.lpSecurityDescriptor = NULL;

	zero(pi);
	zero(si);
	si.cb = sizeof(si);

	/* NOTE: we use PCL_O_CLOEXEC on any handle that gets returned in the exec struct, since
	 * these handles are not for the child process. PCL_O_CLOEXEC is implemented by setting
	 * SECURITY_DESCRIPTOR.bInheritHandle to false when cloexec is set (see pcl_proc_pipe).
	 * Also, the pcl_file_t objects created for the child process are freed after stealing their
	 * win32 handles and assigning them to STARTUPINFO struct. This is for all STDIO handles.
	 * Non-blocking NEVER set on child process handles, only parent (exec struct).
	 */

	/* write to stdin */
	if(flags & (PCL_EXEC_STDIN | PCL_EXEC_STDIN_NB))
	{
		int nonblock = flags & PCL_EXEC_STDIN_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, 0, nonblock | PCL_O_CLOEXEC))
			return SETLASTERR();

		exec->f_stdin = pipes[1];
		si.hStdInput = pipes[0]->fd;
		pipes[0]->fd = PCL_BADFD; /* stolen */
		pcl_file_close(pipes[0]);
	}

	/* read from stdout */
	if(flags & (PCL_EXEC_STDOUT | PCL_EXEC_STDOUT_NB))
	{
		int nonblock = flags & PCL_EXEC_STDOUT_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, nonblock | PCL_O_CLOEXEC, 0))
		{
			SETLASTERR();
			pcl_file_close(exec->f_stdin);
			if(flags & PCL_EXEC_STDIN)
				CloseHandle(si.hStdInput);
			return -1;
		}

		exec->f_stdout = pipes[0];
		si.hStdOutput = pipes[1]->fd;
		pipes[1]->fd = PCL_BADFD; /* stolen */
		pcl_file_close(pipes[1]);
	}

	/* read from stderr */
	if(flags & (PCL_EXEC_STDERR | PCL_EXEC_STDERR_NB))
	{
		int nonblock = flags & PCL_EXEC_STDOUT_NB ? PCL_O_NONBLOCK : 0;

		if(pcl_proc_pipe(pipes, nonblock | PCL_O_CLOEXEC, 0))
		{
			SETLASTERR();
			pcl_file_close(exec->f_stdin);
			pcl_file_close(exec->f_stdout);
			if(flags & PCL_EXEC_STDIN)
				CloseHandle(si.hStdInput);
			if(flags & PCL_EXEC_STDOUT)
				CloseHandle(si.hStdOutput);
			return -1;
		}

		exec->f_stderr = pipes[0];
		si.hStdError = pipes[1]->fd;
		pipes[1]->fd = PCL_BADFD; /* stolen */
		pcl_file_close(pipes[1]);
	}

	int nb = PCL_EXEC_STDIN | PCL_EXEC_STDOUT | PCL_EXEC_STDOUT | PCL_EXEC_STDIN_NB |
		PCL_EXEC_STDOUT_NB | PCL_EXEC_STDERR_NB;

	if(flags & nb)
	{
		si.dwFlags |= STARTF_USESTDHANDLES;

		/* without this, child process will be missing stdout/stderr/stdin
		 * for streams not requested by caller.
		 */
		if(!si.hStdInput)
			si.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
		if(!si.hStdOutput)
			si.hStdOutput = GetStdHandle(STD_OUTPUT_HANDLE);
		if(!si.hStdError)
			si.hStdError = GetStdHandle(STD_ERROR_HANDLE);
	}

	/* parse the command */
	pchar_t **argv = NULL;
	int argc = pcl_proc_parsecmd(exec->command, &argv);
	bool shell = (flags & PCL_EXEC_SHELL) != 0;

	/* (argc*3) = double quotes for each arg + a space. The added 32 is for
	 * any misc stuff like the "/C"...basically a safety net
	 */
	size_t cmd_size = pcl_pcslen(exec->command) + (argc * 3) + 32;
	pchar_t *command = pcl_pmalloc(cmd_size);

	/* If internal command, prepend a cmd.exe "/C" switch */
	pcl_spprintf(command, cmd_size, shell ? _P("/C \"%Ps ") : _P("\"%Ps\" "), argv[0]);

	/* layout the args ... space separated */
	for(int i=1; i < argc; i++)
	{
		pcl_pcscatf(command, cmd_size, _P("%Ps"), argv[i]);
		if(i+1 < argc)
			pcl_pcscatf(command, cmd_size, _P(" "));
	}

	(void) pcl_proc_freeargv(argc, argv);

	/* close double quotes around arg list */
	if(shell)
		pcl_pcscatf(command, cmd_size, _P("\""));

	/* need comspec is shell*/
	pchar_t comspecbuf[512], *comspec = NULL;

	if(shell)
	{
		if(GetEnvironmentVariable(_P("COMSPEC"), comspecbuf, countof(comspecbuf)))
			comspec = comspecbuf;
		if(strempty(comspec))
			comspec = pcl_pcscpy(comspecbuf, countof(comspecbuf), _P("cmd.exe"));
	}

	/* -------------------------------------------------------------------------
	 * Prepend possible exec.libpath to %PATH% env variable. New process
	 * will inherit this, unless CreateProcessAsUser is used within our
	 * createprocess() func, in that case we create an env_block based off
	 * the token of exec.user and we must manually adjust the block..arggg.
	 */

	pchar_t *path = NULL;
	size_t env_len = 0, path_len = 0;

	if(flags & PCL_EXEC_LDPATH && !strempty(exec->ldpath))
	{
		env_len = GetEnvironmentVariable(_P("PATH"), NULL, 0);

		/* allocate enough room for libpath + 1 (';' or '\0') + %PATH%. env_len
		 * includes the NUL already from GetEnvironmentVariable.
		 */
		path_len = pcl_pcslen(exec->ldpath);
		path = pcl_pmalloc(path_len + 1 + env_len);

		/* prepend libpath */
		pcl_pmemcpy(path, exec->ldpath, path_len);
		path[path_len] = 0;

		/* swap ':' for ';'. Since windows uses : for drives "C:\", we have to manually
		 * go through path and exclude drive colons.
		 */
		for(pchar_t *p = path; *p; p++)
		{
			if(*p == _P(':'))
			{
				if(p[1] == PCL_PPATHSEPCHAR)
					p++; // skip it
				else
					*p = _P(';');
			}
		}

		if(env_len > 0)
		{
			/* append ";%PATH%" to get "LIBPATH;%PATH%" */
			path[path_len] = _P(';');
			if(!GetEnvironmentVariable(_P("PATH"), path + path_len + 1, (DWORD)env_len))
			{
				path[path_len] = 0; /* failed, terminate at ';' */
				env_len = 0;
			}
		}

		SetEnvironmentVariable(_P("PATH"), path);
	}

	success = createprocess(exec, comspec, command, &si, &pi);
	pcl_free(command);

	/* reset PATH to what it was prior to changing it (+1 to skip ';') */
	if(env_len > 0)
		SetEnvironmentVariable(_P("PATH"), path + path_len + 1);
	pcl_free_safe(path);

	/* Close our end of child pipes or forever leak! */
	if(flags & (PCL_EXEC_STDIN | PCL_EXEC_STDIN_NB))
		CloseHandle(si.hStdInput);
	if(flags & (PCL_EXEC_STDOUT | PCL_EXEC_STDOUT_NB))
		CloseHandle(si.hStdOutput);
	if(flags & (PCL_EXEC_STDERR | PCL_EXEC_STDERR_NB))
		CloseHandle(si.hStdError);

	if(!success)
	{
		pcl_file_close(exec->f_stdin);
		pcl_file_close(exec->f_stdout);
		pcl_file_close(exec->f_stderr);
		return TRC();
	}

	exec->start = pcl_time();
	exec->pid = (pid_t)pi.dwProcessId;

	if(flags & PCL_EXEC_HANDLE)
		exec->handle = pi.hProcess;
	else
		CloseHandle(pi.hProcess);

	CloseHandle(pi.hThread);
	return 0;
}

static bool
createprocess(pcl_proc_exec_t *exec, const pchar_t *comspec, pchar_t *command,
	STARTUPINFO *si, PROCESS_INFORMATION *pi)
{
	bool success;
	SECURITY_ATTRIBUTES secattrs;
	const pchar_t *langrp = exec->group ? exec->group : _P(".");
	DWORD crflags = CREATE_UNICODE_ENVIRONMENT;

	zero(secattrs);
	secattrs.nLength = sizeof(secattrs);
	secattrs.bInheritHandle = true;

	if(strempty(exec->user))
	{
		if(!CreateProcess(comspec, command, &secattrs, &secattrs, true, crflags,
			NULL, exec->cwd, si, pi))
		{
			return R_SETLASTERR(false);
		}

		return true;
	}

	HANDLE token;
	if(ipcl_win32_logon(exec->user, langrp, exec->pass, &token))
	{
		TRC();
		return false;
	}

	USER_INFO_4 *userinfo;
	pchar_t profdir[512];
	if(!NetUserGetInfo(exec->group, exec->user, 4, (LPBYTE *)&userinfo))
	{
		pcl_pcscpy(profdir, countof(profdir), userinfo->usri4_profile);
		NetApiBufferFree(userinfo);
	}
	else
	{
		DWORD dw = countof(profdir);
		if(!GetUserProfileDirectory(token, profdir, &dw))
			*profdir = 0;
	}

	/* Load user profile since it wasn't done automatically by interactive logon */
	HANDLE hprof = NULL;
	PROFILEINFO profinfo;
	zero(profinfo);
	profinfo.dwSize = sizeof(profinfo);
	profinfo.dwFlags = PI_NOUI;
	profinfo.lpUserName = pcl_pcsdup(exec->user); /* must allocate, IDK, ask M$ */
	profinfo.lpProfilePath = profdir;

	if(LoadUserProfile(token, &profinfo))
		hprof = profinfo.hProfile; /* keep around until ready to unload profile */

	pcl_free(profinfo.lpUserName);

	/* access the directory and executable image in the security context
	 * of the target user, not the caller (us).
	 */
	if(!ImpersonateLoggedOnUser(token))
	{
		SETLASTERR();

		if(hprof)
			UnloadUserProfile(token, hprof);

		CloseHandle(token);

		return false;
	}

	/* Try CreateProcessAsUser, requiring LoadUserProfile and CreateEnvBlock */
	void *env_block = NULL;
	(void)CreateEnvironmentBlock(&env_block, token, true);

	/* CreateEnvironmentBlock can fail, which means process owner won't have
	 * their profile env vars loaded.
	 */
	success = CreateProcessAsUser(token, comspec, command, &secattrs, &secattrs,
		true, crflags | CREATE_NO_WINDOW, env_block, exec->cwd, si, pi);

	if(!success)
		SETLASTERR(); // use this error up until CreateProcessWithLogonW attempt

	RevertToSelf();

	if(env_block)
		DestroyEnvironmentBlock(env_block);

	if(hprof)
		UnloadUserProfile(token, hprof);
	CloseHandle(token);

	if(success)
		return true;

	/* save exception that CreateProcessAsUser threw. */
	pcl_err_freeze(true);

	/* Is the current process running as LocalSystem user? */
	char sidbuf[SECURITY_MAX_SID_SIZE];
	DWORD sidsize = SECURITY_MAX_SID_SIZE;
	PSID process_sid, localsystem_sid = sidbuf;
	CreateWellKnownSid(WinLocalSystemSid, NULL, localsystem_sid, &sidsize);
	if(!(process_sid = ipcl_win32_process_sid()))
	{
		pcl_err_freeze(false);
		return false;
	}

	/* CreateProcessWithLogonW cannot be used if user is LocalSystem. */
	success = EqualSid(localsystem_sid, process_sid);
	pcl_free(process_sid);
	if(success)
	{
		pcl_err_freeze(false);
		return false;
	}

	pcl_err_freeze(false);

	success = CreateProcessWithLogonW(exec->user, langrp, exec->pass,
		LOGON_WITH_PROFILE, comspec, command, crflags, NULL, exec->cwd, si, pi);

	if(!success)
		SETLASTERR();

	return success;
}

