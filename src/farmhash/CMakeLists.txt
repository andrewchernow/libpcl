# Google's farmhash, written in C++ so we separate it into its own target.
# However, the farmhash PCL functions are part of pcl/stdlib.h.

add_library(farmhash OBJECT farmhash32.cpp farmhash64.cpp farmhash128.cpp)

if(UNIX)
	target_compile_options(farmhash PRIVATE "-stdlib=libc++")
else()
	target_compile_options(farmhash PRIVATE "/std:c++latest")
endif()
