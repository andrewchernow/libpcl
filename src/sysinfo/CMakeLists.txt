
add_library(sysinfo OBJECT sysinfo.c)

if(DARWIN)
	target_sources(sysinfo PRIVATE darwin_sysinfo.c)
elseif(LINUX)
	target_sources(sysinfo PRIVATE linux_sysinfo.c)
else()
	target_sources(sysinfo PRIVATE win32_sysinfo.c)
endif()
