
add_library(errctx OBJECT
	err_ctx_clear.c
	err_ctx_clone.c
	err_ctx_create.c
	err_ctx_fprintf.c
	err_ctx_free.c
	err_ctx_serialize.c
	err_ctx_sprintf.c
	err_ctx_trace_free.c
	err_ctx_vfprintf.c
	err_ctx_vsprintf.c)
