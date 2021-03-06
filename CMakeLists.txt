cmake_minimum_required(VERSION 3.4.3...3.17.3)

# set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

# set(CMAKE_VERBOSE_MAKEFILE ON)

project(libpcl
	VERSION 1.0.1
	DESCRIPTION "Portable C Library"
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

include(CheckSymbolExists)

file(REMOVE config.h)
file(APPEND config.h "#ifndef PCL_CONFIG_H\n#define PCL_CONFIG_H\n\n")

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR
		${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "IA64")
	set(ARCH "x86_64")

elseif(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86" OR ${CMAKE_SYSTEM_PROCESSOR} STREQUAL "i386")
	set(ARCH "x86")

else()
	message(FATAL_ERROR "Unsupported architecture ${CMAKE_SYSTEM_PROCESSOR}")
endif()

if(${CMAKE_SYSTEM_NAME} STREQUAL "Darwin")
	set(DARWIN 1)
	set(PLAT "darwin-${ARCH}")

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(WINDOWS 1)
	set(OBJEXT obj)
	set(PLAT "windows-${ARCH}")

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(LINUX 1)
	set(PLAT "linux-${ARCH}")

	# statx available since kernel 4.11. Note: _GNU_SOURCE must be set
	set(CMAKE_REQUIRED_DEFINITIONS -D_GNU_SOURCE)
	check_symbol_exists(statx "sys/stat.h" HAVE_STATX)
	if(HAVE_STATX)
		file(APPEND config.h "#define HAVE_STATX\n")
	endif()

else()
	message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

if(UNIX)
	set(OBJEXT o)

	# available in Linux Kernel 2.6.22 and Darwin 10.13, normally not needed on Darwin
	# since setattrlist() is tried first
	check_symbol_exists(utimensat "sys/stat.h" HAVE_UTIMENSAT)
	if(HAVE_UTIMENSAT)
		file(APPEND config.h "#define HAVE_UTIMENSAT\n")
	endif()
endif()

file(APPEND config.h "\n#endif\n")
add_compile_definitions(PCL_HAVE_CONFIG)

# CFLAGS
if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wpointer-arith -O2 -g0 \
		-Wno-unused-result -fsigned-char -Winline -Wsign-compare -fPIC -fno-strict-aliasing \
		-fomit-frame-pointer -funit-at-a-time -fms-extensions -Wno-microsoft-anon-tag -fvisibility=hidden")

else()
	set(CMAKE_C_FLAGS "/Wall /O2 /utf-8 /wd4201 /wd4710 /wd4668 /wd4255 /wd5045 \
		/wd4464 /wd4820 /wd4996 /wd4214")
endif()

if(DARWIN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
endif()

# CPPFLAGS
if(UNIX)
	add_definitions(-D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_FILE_OFFSET_BITS=64)
endif()

# INC
include_directories(
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs/${PLAT}/include)

if(UNIX)
	include_directories(/usr/local/include)
endif()

add_subdirectory(src)
add_subdirectory(examples)
add_subdirectory(tests)
add_custom_target(doc
	COMMAND doxygen Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Generating API documentation with Doxygen"
	VERBATIM)
