
add_library(thread OBJECT
	mutex_destroy.c
	mutex_init.c
	mutex_lock.c
	mutex_trylock.c
	mutex_unlock.c
	thread_create.c
	thread_id.c
	tls_alloc.c
	tls_free.c
	tls_get.c
	tls_set.c)
