
add_library(file OBJECT
	fdpath.c
	file_close.c
	file_alloc.c
	file_init.c
	file_lock.c
	file_open.c
	file_seek.c
	flagconv.c
	realpath.c
	remove.c
	rename.c
	unlink.c)

if(UNIX)
	target_sources(file PRIVATE
		unix_file_open.c
		unix_read.c
		unix_tryread.c
		unix_trywrite.c
		unix_write.c)
else()
	target_sources(file PRIVATE
		win32_file_open.c
		win32_read.c
		win32_tryio.c
		win32_write.c)
endif()
