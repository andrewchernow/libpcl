
add_library(buf OBJECT
	buf_clear.c
	buf_copy.c
	buf_data.c
	buf_free.c
	buf_get.c
	buf_getint16.c
	buf_getint32.c
	buf_getint64.c
	buf_getstr.c
	buf_grow.c
	buf_init.c
	buf_pos.c
	buf_put.c
	buf_putf.c
	buf_putint16.c
	buf_putint32.c
	buf_putint64.c
	buf_putstr.c
	buf_reset.c
	buf_vputf.c)
