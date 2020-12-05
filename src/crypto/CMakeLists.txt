
add_library(crypto OBJECT
	cipher_create.c
	cipher_final.c
	cipher_free.c
	cipher_name.c
	cipher_reset.c
	cipher_update.c
	digest.c
	digest_create.c
	digest_final.c
	digest_free.c
	digest_len.c
	digest_name.c
	digest_update.c
	rand.c
	cipher_isaead.c)

