
add_library(stat OBJECT strmode.c strflags.c statent.c)

if(UNIX)
	target_sources(stat PRIVATE unix_statent.c)
else()
	target_sources(stat PRIVATE win32_statent.c)
endif()
