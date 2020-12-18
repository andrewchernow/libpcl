
add_library(process OBJECT
	getenv.c
	proc_close.c
	proc_parsecmd.c
	proc_pipe.c
	proc_self.c
	proc_setpriv.c
	setenv.c
	unsetenv.c)

if(WINDOWS)
	target_sources(process PRIVATE win32_proc_exec.c win32_proc_path.c)
else()
	target_sources(process PRIVATE unix_proc_exec.c unix_proc_path.c)
endif()
