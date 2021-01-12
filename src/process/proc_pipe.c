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

#ifdef PCL_WINDOWS
#	include <pcl/crypto.h>
#endif

int
pcl_proc_pipe(pcl_file_t *pipes[2], int rd_oflags, int wr_oflags)
{
	pcl_fd_t fds[2];

#ifdef PCL_WINDOWS
	char name[64];
	uint32_t id[2];
	DWORD busize = 65536;
	DWORD open_mode = PIPE_ACCESS_INBOUND;
	DWORD pipe_mode = PIPE_TYPE_BYTE | PIPE_WAIT;
	DWORD file_flags = FILE_ATTRIBUTE_NORMAL;
	SECURITY_ATTRIBUTES sa = {0};

	pcl_rand(id, (int)sizeof(id));
	sprintf(name, "\\\\.\\Pipe\\%08x%08x%08x", pcl_proc_self(), id[0], id[1]);

	/* create the read end of the pipe (server) */
	sa.nLength = (DWORD)sizeof(sa);
	sa.bInheritHandle = (rd_oflags & PCL_O_CLOEXEC) == 0;
	if(rd_oflags & PCL_O_NONBLOCK)
		open_mode |= FILE_FLAG_OVERLAPPED;

	fds[0] = CreateNamedPipeA(name, open_mode, pipe_mode, 1, busize, busize, 0, &sa);
	if(fds[0] == INVALID_HANDLE_VALUE)
		return SETLASTERR();

	/* create the write end of the pipe (client) */
	sa.bInheritHandle = (wr_oflags & PCL_O_CLOEXEC) == 0;
	if(wr_oflags & PCL_O_NONBLOCK)
		file_flags |= FILE_FLAG_OVERLAPPED;

	fds[1] = CreateFileA(name, GENERIC_WRITE, 0, &sa, OPEN_EXISTING, file_flags, NULL);
	if(fds[1] == INVALID_HANDLE_VALUE)
	{
		SETLASTERR();
		CloseHandle(fds[0]);
		return -1;
	}

#else

	/* pipe2() exists but only on newer linux kernels, osx doesn't appear to
	 * have it at all.  pipe2() allows specifying nonblock and cloexec flags.
	 */
	if(pipe(fds))
		return SETLASTERR();

	for(int i = 0; i < 2; i++)
	{
		int oflags = i == 0 ? rd_oflags : wr_oflags;

		if(oflags & PCL_O_NONBLOCK)
		{
			int n = fcntl(fds[i], F_GETFL);

			if(n < 0)
				SETLASTERR();
			else if((n = fcntl(fds[i], F_SETFL, n | O_NONBLOCK)) < 0)
				SETLASTERR();

			if(n < 0)
			{
				close(fds[0]);
				close(fds[1]);
				return -1;
			}
		}

		if(oflags & PCL_O_CLOEXEC)
		{
			int n = fcntl(fds[i], F_GETFD);

			if(n < 0)
				SETLASTERR();
			else if((n = fcntl(fds[i], F_SETFD, n | FD_CLOEXEC)) < 0)
				SETLASTERR();

			if(n < 0)
			{
				close(fds[0]);
				close(fds[1]);
				return -1;
			}
		}
	}
#endif

	rd_oflags &= ~(PCL_O_WRONLY | PCL_O_RDWR);
	pipes[0] = ipcl_file_alloc(fds[0], 0);
	ipcl_file_init(pipes[0], rd_oflags | PCL_O_RDONLY);

	wr_oflags &= ~(PCL_O_RDONLY | PCL_O_RDWR);
	pipes[1] = ipcl_file_alloc(fds[1], 0);
	ipcl_file_init(pipes[0], wr_oflags | PCL_O_WRONLY);

	return 0;
}
