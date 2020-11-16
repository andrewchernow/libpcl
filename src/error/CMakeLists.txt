
add_library(error OBJECT
	err_clear.c
	err_code.c
	err_ctx.c
	err_fprintf.c
	err_last.c
	err_lastmsg.c
	err_lastos.c
	err_msg.c
	err_name.c
	err_os2pcl.c
	err_osmsg.c
	err_osname.c
	err_set.c
	err_sprintf.c
	err_trace.c
	err_vfprintf.c
	err_vset.c
	err_vsprintf.c
	err_vtrace.c
	errmap.c
	msgmap.c)

if(WINDOWS)
	target_sources(error PRIVATE win32_err_crt2os.c win32_oserrno.c win32_setoserrno.c)
endif()
