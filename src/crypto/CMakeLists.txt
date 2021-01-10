
add_library(crypto OBJECT
	cipher.c
	cipher_final.c
	cipher_free.c
	cipher_name.c
	cipher_reset.c
	cipher_update.c
	digest_value.c
	digest.c
	digest_final.c
	digest_free.c
	digest_len.c
	digest_name.c
	digest_update.c
	rand.c
	cipher_isaead.c)

