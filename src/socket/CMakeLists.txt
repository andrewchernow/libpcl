
add_library(socket OBJECT socket.c socket_alloc.c connect.c socket_error.c
	shutdown.c socket_isalive.c socket_close.c accept.c socket_ispassive.c select.c
	recv.c recvall.c send.c sendall.c socket_setnonblocking.c socket_setrcvtimeo.c
	socket_setsndtimeo.c socket_port.c socket_ip.c bind.c listen.c socket_setkeepalive.c
	socket_fd.c)

if(WINDOWS)
	target_sources(socket PRIVATE win32_socket_handler.c)
endif()
