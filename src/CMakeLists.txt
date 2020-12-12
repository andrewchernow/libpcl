
# shared and static: https://stackoverflow.com/questions/2152077/is-it-possible-to-get-cmake-to-build-both-a-static-and-shared-version-of-the-sam


# sub targets
add_subdirectory(alloc)
add_subdirectory(atomic)
add_subdirectory(buf)
add_subdirectory(crypto)
add_subdirectory(dir)
add_subdirectory(dl)
add_subdirectory(error)
add_subdirectory(event)
add_subdirectory(farmhash)
add_subdirectory(file)
add_subdirectory(htable)
add_subdirectory(init)
add_subdirectory(io)
add_subdirectory(json)
add_subdirectory(log)
add_subdirectory(net)
add_subdirectory(process)
add_subdirectory(socket)
add_subdirectory(ssl)
add_subdirectory(stat)
add_subdirectory(string)
add_subdirectory(strint)
add_subdirectory(sysinfo)
add_subdirectory(thread)
add_subdirectory(time)
add_subdirectory(usrgrp)
add_subdirectory(vector)

if(WIN32)
	add_subdirectory(win32)
else()
	add_subdirectory(unix)
endif()

# pcl library target and all sources
add_library(pcl SHARED
	$<TARGET_OBJECTS:alloc>
	$<TARGET_OBJECTS:atomic>
	$<TARGET_OBJECTS:buf>
	$<TARGET_OBJECTS:crypto>
	$<TARGET_OBJECTS:dir>
	$<TARGET_OBJECTS:dl>
	$<TARGET_OBJECTS:error>
	$<TARGET_OBJECTS:event>
	"${PROJECT_SOURCE_DIR}/libs/${PLAT}/farmhash.${OBJEXT}"
	$<TARGET_OBJECTS:farmhash>
	$<TARGET_OBJECTS:file>
	$<TARGET_OBJECTS:htable>
	$<TARGET_OBJECTS:init>
	$<TARGET_OBJECTS:io>
	$<TARGET_OBJECTS:json>
	$<TARGET_OBJECTS:log>
	$<TARGET_OBJECTS:net>
	$<TARGET_OBJECTS:process>
	$<TARGET_OBJECTS:socket>
	$<TARGET_OBJECTS:ssl>
	$<TARGET_OBJECTS:stat>
	$<TARGET_OBJECTS:string>
	$<TARGET_OBJECTS:strint>
	$<TARGET_OBJECTS:sysinfo>
	$<TARGET_OBJECTS:thread>
	$<TARGET_OBJECTS:time>
	$<TARGET_OBJECTS:usrgrp>
	$<TARGET_OBJECTS:vector>)

# this is the default on unixes but not windows. import_prefix only used on windows
set_target_properties(pcl PROPERTIES PREFIX "lib" IMPORT_PREFIX "lib")

if(WIN32)
	target_sources(pcl PRIVATE $<TARGET_OBJECTS:win32>)
	target_link_libraries(pcl
		${PROJECT_SOURCE_DIR}/libs/${PLAT}/pthreadVC3.lib
		libcrypto.lib
		libssl.lib
		Ws2_32.lib
		Shlwapi.lib
		Netapi32.lib
		Userenv.lib
		Dnsapi.lib)

else()
	target_sources(pcl PRIVATE $<TARGET_OBJECTS:unix>)

	if(DARWIN)
		target_link_options(pcl PRIVATE -L/usr/local/lib -lm -lpthread -ldl -lresolv -lcrypto -lssl)
	endif()
endif()







#target_include_directories(pcl PUBLIC ${PROJECT_SOURCE_DIR}/include)

#target_include_directories(pcl SYSTEM PRIVATE ${PROJECT_SOURCE_DIR}/libs/${PLAT}/include)

# -Werror
#target_compile_options(pcl PRIVATE
#	-Wall -Wextra -Wpedantic -Wpointer-arith -O2 -g0 -fsigned-char -Winline
#	-Wsign-compare -fno-strict-aliasing -fomit-frame-pointer -funit-at-a-time)

#if(UNIX)
#	target_compile_definitions(pcl PRIVATE _GNU_SOURCE _REENTRANT _THREAD_SAFE _FILE_OFFSET_BITS=64)
#endif()

#if(DARWIN)
#	target_compile_options(pcl PRIVATE -fvisibility=default -arch x86_64)
#	target_link_options(pcl PRIVATE -fvisibility=default)
#elseif(LINUX)
#	target_link_options(pcl PRIVATE -lrt)
#endif()

#target_link_options(pcl PRIVATE -lm -lpthread -ldl -lresolv)
#target_link_libraries(pcl PRIVATE -L${PROJECT_SOURCE_DIR}/libs/${PLAT} crypto ssl)
