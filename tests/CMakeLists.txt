
if(LINUX)
	link_libraries(pcl -lm -lpthread -ldl -lresolv -lcrypto -lssl)
else()
	link_libraries(pcl)
endif()

add_executable(test test.c
	array.c
	buf.c
	crypto.c
	dir.c
	htable.c
	json.c time.c)

if(LINUX)
	# needed to find symbols within current executable
	target_link_options(test PRIVATE -rdynamic)
endif()
