# Google's farmhash, written in C++ so we separate it into its own target.
# However, the farmhash PCL functions are part of pcl/stdlib.h.

add_library(farmhash OBJECT farmhash32.cpp farmhash64.cpp farmhash128.cpp)

if(DARWIN)
	target_compile_options(farmhash PRIVATE "-stdlib=libc++")
elseif(WINDOWS)
	target_compile_options(farmhash PRIVATE "/std:c++latest")
endif()
