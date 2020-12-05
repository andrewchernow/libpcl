
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

#include "../time/_time.h"     // time_handler
#include "../event/_event.h"   // ipcl_event_init
#include "../errctx/_errctx.h" // err_ctx_handler
#include <pcl/init.h>
#include <pcl/atomic.h>

#ifdef PCL_WINDOWS
#	include "stat/_stat.h"     // win32_stat_handler
#	include "socket/_socket.h" // socket_handler
#	include <pcl/file.h>       // STDxx_FILENO
#	include <fcntl.h>
#	include <io.h>
#	include <stdlib.h>
#else
#	include <sys/stat.h>       // umask
#endif

#include <locale.h>

static pcl_event_handler_t builtin_handlers[] = {
	ipcl_err_ctx_handler,
	ipcl_time_handler,
#ifdef PCL_WINDOWS
	ipcl_win32_socket_handler,
	ipcl_win32_stat_handler
#endif
};

void
pcl_init(void)
{
	static pcl_atomic_t pcl_initialized = 0;

	if(pcl_atomic_compare_exchange(&pcl_initialized, 0, 1)) /* if(library_initialized == 1) */
		return;

#ifdef PCL_WINDOWS
	_set_fmode(_O_BINARY);
	_setmode(STDIN_FILENO, _O_BINARY);
	_setmode(STDOUT_FILENO, _O_BINARY);
	_setmode(STDERR_FILENO, _O_BINARY);
	const char *locale = "";
#else
	umask(0);
	const char *locale = "en_US.UTF-8";
#endif

	(void) setlocale(LC_ALL, locale);
	ipcl_event_init();

	/* register builtin handlers */
	for(int i = 0; i < countof(builtin_handlers); i++)
		pcl_event_register(builtin_handlers[i]);

	/* dispatch library init event */
	pcl_event_dispatch(PCL_EVENT_INIT, NULL);
}
