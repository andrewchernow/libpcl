#project(libpcl_examples LANGUAGES C)
#cmake_minimum_required(VERSION 3.4.3...3.17.3)
#set(CMAKE_OSX_DEPLOYMENT_TARGET 10.6)

add_executable(https https.c)
target_link_libraries(https PRIVATE pcl)
