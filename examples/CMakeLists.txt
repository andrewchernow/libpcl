
if(LINUX)
	link_libraries(pcl -lm -lpthread -ldl -lresolv -lcrypto -lssl)
else()
	link_libraries(pcl)
endif()

add_executable(ex_getopt getopt.c)

add_executable(ex_json json.c)

add_executable(ex_buf buf.c)
add_executable(ex_cipher cipher.c)
add_executable(ex_digest digest.c)
add_executable(ex_error error.c)
add_executable(ex_exec exec.c)
add_executable(ex_htable htable.c)
add_executable(ex_https https.c)
add_executable(ex_ls ls.c)
add_executable(ex_sysinfo sysinfo.c)

