cmake_minimum_required(VERSION 3.4.3...3.17.3)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.7)

project(libpcl
	VERSION 1.0.1
	DESCRIPTION "Portable C Library"
	LANGUAGES C CXX)

set(CMAKE_C_STANDARD 11)

if(${CMAKE_SYSTEM_PROCESSOR} STREQUAL "x86_64" OR
	${CMAKE_SYSTEM_PROCESSOR} STREQUAL "AMD64" OR
	${CMAKE_SYSTEM_PROCESSOR} STREQUAL "IA64")
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
	add_compile_definitions(PCL_UNIX PCL_EXPORT=extern PCL_INLINE=__inline__)
endif()
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
add_subdirectory(src)
add_subdirectory(tests)
