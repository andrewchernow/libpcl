cmake_minimum_required(VERSION 3.4.3...3.17.3)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.9)

# set(CMAKE_VERBOSE_MAKEFILE ON)

project(libpcl
	VERSION 1.0.1
	DESCRIPTION "Portable C Library"
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

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
	add_compile_definitions(PCL_DARWIN)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Windows")
	set(WINDOWS 1)
	set(OBJEXT obj)
	set(PLAT "windows-${ARCH}")
	add_compile_definitions(PCL_WINDOWS "PCL_EXPORT=__declspec(dllexport)" PCL_INLINE=__inline
		_UNICODE UNICODE WIN32_LEAN_AND_MEAN VC_EXTRALEAN)

elseif(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(LINUX 1)
	set(PLAT "linux-${ARCH}")
	add_compile_definitions(PCL_LINUX)

else()
	message(FATAL_ERROR "Unsupported platform ${CMAKE_SYSTEM_NAME}")
endif()

if(UNIX)
	set(OBJEXT o)
	add_compile_definitions(PCL_UNIX PCL_EXPORT=extern PCL_INLINE=__inline__)
endif()
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)


# CFLAGS
if(UNIX)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic -Wpointer-arith -O2 -g0 \
		-Wno-unused-result -fsigned-char -Winline -Wsign-compare -fPIC -fno-strict-aliasing \
		-fomit-frame-pointer -funit-at-a-time")

else()
	set(CMAKE_C_FLAGS "/Wall /O2 /wd4668 /wd4255 /wd5045 /wd4464 /wd4820 /wd4996 /wd4214")
endif()

if(DARWIN)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=default")
endif()

# CPPFLAGS
if(UNIX)
	add_definitions(-D_GNU_SOURCE -D_REENTRANT -D_THREAD_SAFE -D_FILE_OFFSET_BITS=64)
endif()

# INC
include_directories(
	${PROJECT_SOURCE_DIR}/include
	${PROJECT_SOURCE_DIR}/libs/${PLAT}/include)

if(UNIX)
	include_directories(/usr/local/include)
endif()

add_subdirectory(src)
add_subdirectory(examples)

add_custom_target(doc
	COMMAND doxygen Doxyfile
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
	COMMENT "Generating API documentation with Doxygen"
	VERBATIM)