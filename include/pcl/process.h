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
 	OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef LIBPCL_PROCESS_H
#define LIBPCL_PROCESS_H

/** @defgroup proc Process Management
 * This module contains functions for managing processes.
 * @{
 */
#include <pcl/time.h>

/** Get a write handle to the spawned process's \c STDIN.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stdin and must be closed by
 * the caller after use. If ::PCL_EXEC_STDIN_NB is also set, this flag is ignored.
 * @see PCL_EXEC_STDIN_NB
 */
#define PCL_EXEC_STDIN        0x0001

/** Get a non-blocking write handle to the spawned process's \c STDIN.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stdin and must be closed by
 * the caller after use. If this is set, it takes precedence over ::PCL_EXEC_STDIN.
 * @see PCL_EXEC_STDIN
 */
#define PCL_EXEC_STDIN_NB     0x0002

/** Get a read handle to the spawned process's \c STDOUT.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stdout and must be closed by
 * the caller after use. If ::PCL_EXEC_STDOUT_NB is also set, this flag is ignored.
 * @see PCL_EXEC_STDOUT_NB
 */
#define PCL_EXEC_STDOUT       0x0004

/** Get a non-blocking read handle to the spawned process's \c STDIN.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stdin and must be closed by
 * the caller after use. If this is set, it takes precedence over ::PCL_EXEC_STDOUT.
 * @see PCL_EXEC_STDOUT
 */
#define PCL_EXEC_STDOUT_NB    0x0008

/** Get a read handle to the spawned process's \c STDERR.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stderr and must be closed by
 * the caller after use. If ::PCL_EXEC_STDERR_NB is also set, this flag is ignored.
 * @see PCL_EXEC_STDERR_NB
 */
#define PCL_EXEC_STDERR       0x0010

/** Get a non-blocking read handle to the spawned process's \c STDERR.
 * The ::pcl_file_t (handle) is stored in pcl_proc_exec_t.f_stderr and must be closed by
 * the caller after use. If this is set, it takes precedence over ::PCL_EXEC_STDERR.
 * @see PCL_EXEC_STDERR
 */
#define PCL_EXEC_STDERR_NB    0x0020

/** Indicates that the command to exec should be passed to the SHELL. On unix, this uses
 * `$SHELL -c "your_command"` and on windows, this uses `%COMSPEC% /C "your_command`.
 *
 * This is only required for internal (builtin) commands. These are commands
 * that are built into windows CMD.EXE or the Unix shell: like BASH `cd`.
 * Thus, you must do `cmd.exe /C "command"` or `sh -c "command"` to exec them. This
 * will work for non-built-in commands but incurs an unnecessary shell.
 */
#define PCL_EXEC_SHELL        0x0040

/** Get a raw operating system handle to the spawned process.
 * This will be written to pcl_proc_exec_t.handle. It is a windows process \c HANDLE or a
 * Unix \c PID. This must be passed to ::pcl_proc_close when no longer needed. On Unix, the
 * ::pcl_proc_close doesn't do anything but for cross-platform applications, it should
 * be called to avoid preprocessing.
 */
#define PCL_EXEC_HANDLE       0x0080

/** Sets the library search path within the new process.
 * The library path is set to the value of pcl_proc_exec_t.ldpath; a colon separated list of
 * paths. On windows colons are replaced for ';' so either can be used.
 *
 * On Unix, if 'ldpath' is \c NULL, exec tries pcl_proc_exec_t.cwd and then \c getcwd. If none of
 * these work, the flag is ignored.
 *
 * On windows, if pcl_proc_exec_t.ldpath is \c NULL, pcl_proc_exec ignores this flag (continues)
 * and there are no fallback attempts like on Unix. This is because on windows, the current
 * working directory is already part of the library search path.
 *
 * pcl_proc_exec_t.ldpath must be \c NULL for exec to try another one. An empty string is
 * completely valid and can be used to clear the library search path for this new process; but
 * not effect the system-wide search paths.
 *
 *  * Windows: added to PATH => https://docs.microsoft.com/en-us/cpp/build/search-path-used-by-windows-to-locate-a-dll
 *  * Linux: added to LD_LIBRARY_PATH
 *  * Darwin: added to DYLD_LIBRARY_PATH
 */
#define PCL_EXEC_LDPATH       0x0100

/* ---------------------------------------------------------------------------
 * Process/thread privileges.  see pcl_proc_setpriv
 * NOTE: These privs have no effect on Unix machines at this time.
 *
 * Windows supports any priv in winnt.h or at this URL:
 *   http://msdn.microsoft.com/en-us/library/windows/desktop/bb530716(v=vs.85).aspx
 */

/** Grant all read access to files, bypassing access checks.  Great for
 * backup operations.
 * @see pcl_proc_setpriv
 */
#define PRIV_RDFILE    _P("SeBackupPrivilege")

/** Grant all write access to files, bypassing access checks.  Great for
 * restore operations.
 * @see pcl_proc_setpriv
 */
#define PRIV_WRFILE   _P("SeRestorePrivilege")

/** Grant all access to processes owned by other accounts.
 * This is required to use PCL's process list/stat API.
 * @see pcl_proc_setpriv
 */
#define PRIV_PROCESS   _P("SeDebugPrivilege")

/** Grant the ability to make (create) symbolic links.
 * @see pcl_proc_setpriv
 */
#define PRIV_MKSYMLINK _P("SeCreateSymbolicLinkPrivilege")

#ifdef __cplusplus
extern "C" {
#endif

/** Contains input and output parameters for a call to pcl_proc_exec(). */
typedef struct
{
	/* input parameters */

	/** command and arguments: "ls -la /home/username" */
	const pchar_t *command;

	/** run as user name */
	const pchar_t *user;

	/** runs as Windows LAN group (domain/workgroup) or Unix group */
	const pchar_t *group;

	/** Windows only...account password */
	const pchar_t *pass;

	/** set current working directory of new process. */
	const pchar_t *cwd;

	/** library search path to use for exec'd process, requires PCL_EXEC_LDPATH. For more info,
	 * see PCL_EXEC_LDPATH. Set this to an empty string "" to clear the search path for new
	 * process and only use the system defaults.
	 */
	const pchar_t *ldpath;

	/* output parameters */

	/** set to the process id if spawned process. This is always set. */
	pid_t pid;

	/** start time of child process. This is always set. */
	pcl_time_t start;

	/** set to the process handle (windows HANDLE and unix pid). This must be passed to
	 * ::pcl_proc_close when no longer needed. If ::PCL_EXEC_HANDLE is not set, this will
	 * be \c NULL.
	 */
	pcl_prochandle_t handle;

	/** Set to the read end of the child process's stdout when ::PCL_EXEC_STDOUT or
	 * ::PCL_EXEC_STDOUT_NB. This is \c NULL otherwise. This must be closed with
	 * ::pcl_file_close.
	 */
	pcl_file_t *f_stdout;

	/** Set to the write end of the child process's stdin when ::PCL_EXEC_STDIN or
	 * ::PCL_EXEC_STDIN_NB. This is \c NULL otherwise. This must be closed with
	 * ::pcl_file_close.
	 */
	pcl_file_t *f_stdin;

	/** Set to the read end of the child process's stderr when ::PCL_EXEC_STDERR or
	 * ::PCL_EXEC_STDERR_NB. This is \c NULL otherwise. This must be closed with
	 * ::pcl_file_close.
	 */
	pcl_file_t *f_stderr;
} pcl_proc_exec_t;

/** Execute a command within a child process.
 * @param exec
 * @param flags bitmask of \c PROC_EXEC_xxx flags
 * @return 0 on success and -1 on error.
 */
PCL_EXPORT int pcl_proc_exec(pcl_proc_exec_t *exec, int flags);

/** Creates a pipe pair of pcl_file_t objects using the given read and write
 * open flags (PCL_O_xxx). Note that unlike the unix `pipe2` function, this
 * version allows you to set different flags for both the read and write
 * ends of the pipe. pipes[0]=read and pipes[1]=write. Again, the rd_oflags and
 * wr_oflags are PCL_O_xxx open flags, not POSIX O_xxx flags. PCL_O_RDONLY is
 * automatically added to the rd_oflags and PCL_O_WRONLY to wr_oflags. PCL_O_RDWR is
 * removed if present.
 * @param pipes a two element array of ::pcl_file_t pointers. Element 0 is set to the read
 * end of the pipe and element 1 to the write end.
 * @param rd_oflags a bitmask of PCL_O_xxx flags for the read end of the pipe
 * @param wr_oflags a bitmask of PCL_O_xxx flags for the write end of the pipe
 * @return 0 on success and -1 on error
 */
PCL_EXPORT int pcl_proc_pipe(pcl_file_t *pipes[2], int rd_oflags, int wr_oflags);

/** Gets the Process ID of the calling process (PID).
 * @return process id
 */
PCL_EXPORT pid_t pcl_proc_self(void);

/** Get a process's path.
 * @param pid process id
 * @return pointer to the allocated process image path or \c NULL on error. This value must
 * be freed.
 */
PCL_EXPORT pchar_t *pcl_proc_path(pid_t pid);

/** Enables or disables a process privilege.  'priv' can be any privilege
 * defined in winnt.h and supported by LookupPrivilegeValue on windows.
 * Currently, only one of the PRIV_xxx constants are known to PCL if libpcl
 * ever decides to provide functionality for non-Windows based machines.
 * @param priv privilege name
 * @param enable true to enable and false to disable
 * @return 0 on success and -1 on error.  Unix system always return zero
 * and currently provides no functionality.
 */
PCL_EXPORT int pcl_proc_setpriv(const pchar_t *priv, bool enable);

/** Close a process handle.
 * @note this does nothing on unixes.
 * @param handle wijndows \c HANDLE and Unix pid.
 */
PCL_EXPORT void pcl_proc_close(pcl_prochandle_t handle);

/** Converts the given shell command to a valid argv array, which is \c NULL terminated.
 * @note a blank shell_cmd, will result in zero being returned which could be an error to an
 * application.
 * @param shell_cmd pointer to the shell command to parse
 * @return pointer to an array of arguments with a NULL as the last element. One can simply
 * access pcl_array_t.elements for an \c argv style array.
 */
PCL_EXPORT pcl_array_t *pcl_proc_parsecmd(const pchar_t *shell_cmd);

/** Get the value of an environment variable.
 * @param name name of the environment variable
 * @return pointer to the environment variable's value. This is an allocated value that
 * must be freed. If \a name was not found, \c NULL is returned.
 */
PCL_EXPORT pchar_t *pcl_getenv(const pchar_t *name);

/** Set the value of an environment variable.
 * @param name name of the environment variable
 * @param value pointer to the value to set
 * @param overwrite When true, if \a name already exists it will be overwritten. If this is
 * false, the value will not be set. If \a name doesn't exist, this argument is ignored.
 * @return o on success and -1 on error
 */
PCL_EXPORT int pcl_setenv(const pchar_t *name, const pchar_t *value, bool overwrite);

/** Unset an environment variable.
 * @param name name of the environment variable
 * @return o on success and -1 on error
 */
PCL_EXPORT int pcl_unsetenv(const pchar_t *name);

#ifdef __cplusplus
}
#endif

/** @} */
#endif
