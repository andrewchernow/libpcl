
add_library(htable OBJECT
	htable_chkcapacity.c
	htable_clear.c
	htable.c
	htable_free.c
	htable_get.c
	htable_keys.c
	htable_lookup.c
	htable_put.c
	htable_rehash.c
	htable_remove.c
	htable_init.c htable_iter.c)
