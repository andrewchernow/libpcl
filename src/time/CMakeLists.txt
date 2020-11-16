
add_library(time OBJECT
	break_down_time.c
	clock.c
	context.c
	mktime.c
	sleep.c
	strftime.c
	time.c)

if(UNIX)
	target_sources(time PRIVATE unix_utimes.c)
else()
	target_sources(time PRIVATE win32_ftime_to_pcl.c win32_pcl_to_ftime.c win32_utimes.c)
endif()
