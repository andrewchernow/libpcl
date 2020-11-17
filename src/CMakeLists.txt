
# shared and static: https://stackoverflow.com/questions/2152077/is-it-possible-to-get-cmake-to-build-both-a-static-and-shared-version-of-the-sam

# CFLAGS
if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wpointer-arith -O2 -g0 \
		-fsigned-char -Winline -Wsign-compare -fno-strict-aliasing -fomit-frame-pointer -funit-at-a-time")
else()
	set(CMAKE_C_FLAGS "/Wall /wd4668 /wd4255 /wd5045 /wd4464 /wd4820 /wd4996 /wd4214")
endif()

if(DARWIN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=default -arch x86_64")
endif()

# CPPFLAGS
if(UNIX)
	add_definitions(-D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_FILE_OFFSET_BITS=64)
endif()

# INC
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs/${PLAT}/include)

#if(WINDOWS)
#	include_directories("C:/Program Files/OpenSSL-Win64/include")
#endif()

# sub targets
add_subdirectory(atomic)
add_subdirectory(buf)
add_subdirectory(dir)
add_subdirectory(dl)
add_subdirectory(errctx)
add_subdirectory(error)
add_subdirectory(event)
add_subdirectory(farmhash)
add_subdirectory(file)
add_subdirectory(htable)
add_subdirectory(log)
add_subdirectory(net)
add_subdirectory(process)
add_subdirectory(socket)
add_subdirectory(ssl)
add_subdirectory(stat)
add_subdirectory(stdio)
add_subdirectory(stdlib)
add_subdirectory(string)
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
add_library(pcl STATIC
	$<TARGET_OBJECTS:atomic>
	$<TARGET_OBJECTS:buf>
	$<TARGET_OBJECTS:dir>
	$<TARGET_OBJECTS:dl>
	$<TARGET_OBJECTS:errctx>
	$<TARGET_OBJECTS:error>
	$<TARGET_OBJECTS:event>
	$<TARGET_OBJECTS:farmhash>
	$<TARGET_OBJECTS:file>
	$<TARGET_OBJECTS:htable>
	$<TARGET_OBJECTS:log>
	$<TARGET_OBJECTS:net>
	$<TARGET_OBJECTS:process>
	$<TARGET_OBJECTS:socket>
	$<TARGET_OBJECTS:ssl>
	$<TARGET_OBJECTS:stat>
	$<TARGET_OBJECTS:stdio>
	$<TARGET_OBJECTS:stdlib>
	$<TARGET_OBJECTS:string>
	$<TARGET_OBJECTS:sysinfo>
	$<TARGET_OBJECTS:thread>
	$<TARGET_OBJECTS:time>
	$<TARGET_OBJECTS:usrgrp>
	$<TARGET_OBJECTS:vector>)

if(WIN32)
	target_sources(pcl PRIVATE $<TARGET_OBJECTS:win32>)
else()
	target_sources(pcl PRIVATE $<TARGET_OBJECTS:unix>)
endif()

target_sources(pcl PRIVATE init.c)

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
