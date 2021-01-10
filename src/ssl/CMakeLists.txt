
add_library(ssl OBJECT
	openssl_error.c
	ssl_accept.c
	ssl_certentry.c
	ssl_close.c
	ssl_configure_socket.c
	ssl_connect.c
	ssl.c
	ssl_error.c
	ssl_free.c
	ssl_flags.c
	ssl_handshake.c
	ssl_isalive.c
	ssl_percert.c
	ssl_recv.c
	ssl_send.c
	ssl_setciphlist.c
	ssl_setpair.c
	ssl_setpeerca.c
	ssl_socket.c)
