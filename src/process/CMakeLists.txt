
add_library(process OBJECT
	proc_close.c
	proc_freecmd.c
	proc_parsecmd.c
	proc_path.c
	proc_pipe.c
	proc_self.c
	proc_setpriv.c)

if(WINDOWS)
	target_sources(process PRIVATE win32_proc_exec.c)
else()
	target_sources(process PRIVATE unix_proc_exec.c)
endif()
