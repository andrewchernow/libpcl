
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

#ifndef LIBPCL_PROCESS_H
#define LIBPCL_PROCESS_H

#include <pcl/time.h>

/* ---------------------------------------------------------------------------
 * pcl_proc_exec flags
 */

/* Get a write handle to stdin (pcl_proc_exec_t.f_stdin). This must be closed
 * via pcl_close().
 */
#define PCL_PREX_STDIN        0x0001

/* Get a read handle to stdout (pcl_proc_exec_t.f_stdout). This must be closed
 * via pcl_close().
 */
#define PCL_PREX_STDOUT       0x0002

/* Get a read handle to stderr (pcl_proc_exec_t.f_stderr). This must be closed
 * via pcl_close().
 */
#define PCL_PREX_STDERR       0x0004

/* Indicates that the pcl_proc_exec command should be passed to the SHELL.
 * On unix, this uses `$SHELL -c "your_command"` and on windows, this uses
 *
 *   %COMSPEC% /C "your_command"
 *
 * This is only required for internal (builtin) commands. These are commands
 * that are built into windows CMD.EXE or the Unix shell: like BASH `cd`.
 * Thus, you must do `cmd.exe /C "command"` or `sh -c "command"` to exec them.
 */
#define PCL_PREX_SHELL        0x0008

/* If PCL_PREX_STDIN, PCL_PREX_STDOUT or PCL_PREX_STDERR are set, open them in
 * non-blocking mode.  Otherwise, this has no effect. Setting this flag will
 * cause all standard streams to be non-blocking.
 */
#define PCL_PREX_NONBLOCK     0x0010

/* Populates the pcl_proc_exec_t.handle member with the process handle. This is
 * only meaningful for Windows, it is the process HANDLE of the process
 * pcl_proc_exec() just spawned. It is just the PID on Unix. On Windows, this
 * HANDLE must be closed via pcl_proc_close(). On unix, pcl_proc_close() does
 * nothing, although calling it in a multi-platform application is best practice.
 * It keeps the app non-specific to any platform.
 */
#define PCL_PREX_HANDLE       0x0020

/* Sets the library search path within the new process. The library path is set
 * to the value of pcl_proc_exec_t.ldpath; a colon separated list of paths (semi-colon
 * on windows). If 'ldpath' is NULL, on Unix pcl_proc_exec tries pcl_proc_exec_t.cwd and
 * then `getcwd`. If none of those work, pcl_proc_exec() ignores this flag and continues.
 * On windows, if pcl_proc_exec_t.ldpath is NULL, pcl_proc_exec ignores this flag (continues)
 * and there are no fallback attempts like on Unix. This is because on windows,
 * the current working directory is already part of the library search path.
 *
 * These values must be NULL for pcl_proc_exec to try another one or assume they are
 * unset. An empty string is completely valid and can be used to clear the library
 * search path for this new process; but not effect the system-wide search paths.
 * If no value can be obtained while this flag is set, the pcl_proc_exec call will
 * give up and continue as if this flag was not set.
 *
 * Windows: added to PATH
 *  > https://docs.microsoft.com/en-us/cpp/build/search-path-used-by-windows-to-locate-a-dll
 * Linux:   added to LD_LIBRARY_PATH
 * Darwin:  added to DYLD_LIBRARY_PATH
 */
#define PCL_PREX_LDPATH       0x0040

/* same as PCL_O_CLOEXEC or PCL_O_NOINHERIT */
#define PCL_PREX_CLOEXEC      0x0080

/* ---------------------------------------------------------------------------
 * Process/thread privileges.  see pcl_proc_setpriv
 * NOTE: These privs have no effect on Unix machines at this time.
 *
 * Windows supports any priv in winnt.h or at this URL:
 *   http://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx
 */

/* Grant all read access to files, bypassing access checks.  Great for
 * backup operations.
 */
#define PRIV_RDFILE    _T("SeBackupPrivilege")

/* Grant all write access to files, bypassing access checks.  Great for
 * restore operations.
 */
#define PRIV_WRFILE   _T("SeRestorePrivilege")

/* Grant all access to processes owned by other accounts.
 * This is required to use PCL's process list/stat API.
 */
#define PRIV_PROCESS   _T("SeDebugPrivilege")

/* Grant the ability to make (create) symbolic links. */
#define PRIV_MKSYMLINK _T("SeCreateSymbolicLinkPrivilege")

#ifdef __cplusplus
extern "C" {
#endif

/* Contains input and output parameters for a call to pcl_proc_exec().
 *
 * Output params are populated by pcl_proc_exec().  Contains information
 * about the process pcl_proc_exec() spawned, including optional standard
 * input, output and error redirected file handles.  See PCL_PREX_xxx flags.
 */
typedef struct
{
	/* input parameters */
	const tchar_t *command; /* command and arguments: "ls -la /home/foobar" */
	const tchar_t *user;    /* run as user name */
	const tchar_t *group;   /* runs as Windows LAN group (domain/workgroup) or Unix group */
	const tchar_t *pass;    /* Windows only...account password */
	const tchar_t *cwd;     /* set current working directory of new process. */
	const tchar_t *ldpath;  /* library search path to use for exec'd process, requires
	                           PCL_PREX_LDPATH. For more info, see PCL_PREX_LDPATH. Set this
	                           to an empty string "" to clear the search path for new
	                           process and only use the system defaults. */

	/* output parameters */
	pid_t pid;               /* always set */
	pcl_time_t start;        /* always set */
	pcl_prochandle_t handle; /* PCL_PREX_HANDLE (pcl_proc_close), NULL means not set */
	pcl_file_t *f_stdout;    /* PCL_PREX_STDOUT, NULL means not set */
	pcl_file_t *f_stdin;     /* PCL_PREX_STDERR, NULL means not set */
	pcl_file_t *f_stderr;    /* PCL_PREX_STDIN, NULL means not set */
} pcl_proc_exec_t;

PCL_EXPORT int
pcl_proc_exec(pcl_proc_exec_t *exec, int flags);

/* Creates a pipe pair of pcl_file_t objects using the given read and write
 * open flags (PCL_O_xxx). Note that unlike the unix `pipe2` function, this
 * version allows you to set different flags for both the read and write
 * ends of the pipe. pipes[0]=read and pipes[1]=write. Again, the rd_oflags and
 * wr_oflags are PCL_O_xxx open flags, not POSIX O_xxx flags.
 */
PCL_EXPORT int
pcl_proc_pipe(pcl_file_t *pipes[2], int rd_oflags, int wr_oflags);

/* Gets the Process ID of the calling process (PID). Same as calling
 * the POSIX `getpid`.
 */
PCL_EXPORT pid_t
pcl_proc_self(void);

/* Gets the file system path for the given process id (process image path).
 * If pid is < 0, the path for the currently running process is obtained.
 * Returns the number of chars written to buf (excluding NUL) on success
 * and zero on error.
 */
PCL_EXPORT int
pcl_proc_path(pid_t pid, tchar_t *buf, size_t len);

/* Enables or disables a process privilege.  'priv' can be any privilege
 * defined in winnt.h and supported by LookupPrivilegeValue on windows.
 * Currently, only one of the PRIV_xxx constants are known to PCL if libpcl
 * ever decides to provide functionality for non-Windows based machines.
 *
 * Returns zero on success and -1 on error.  Unix system always return zero
 * and currently provide no functionality.
 */
PCL_EXPORT int
pcl_proc_setpriv(const tchar_t *priv, bool enable);

PCL_EXPORT void
pcl_proc_close(pcl_prochandle_t handle);

/* Converts the given shell command to a valid argv array, which is
 * NULL terminated. This returns the number of arguments, exlcuding terminating
 * NULL element, or -1 on error.  NOTE: a blank shell_cmd, will result in
 * zero being returned which could be an error to an application but not
 * all so this function doesn't raise an exception for this case. A NULL
 * 'shell_cmd' is treated as an PCL_EINVAL error.
 *
 * On return, *out will point to a valid argv[] that can be passed to a
 * Unix execv function. Use pcl_proc_freeargv() to free *out.
 */
PCL_EXPORT int
pcl_proc_parsecmd(const tchar_t *shell_cmd, tchar_t ***out);

/* always returns NULL. This can be used with any pointer table. */
PCL_EXPORT void *
pcl_proc_freeargv(int targc, tchar_t **targv);

#ifdef __cplusplus
}
#endif
#endif
