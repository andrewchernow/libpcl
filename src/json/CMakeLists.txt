
add_library(json OBJECT
	json_decode.c
	json_encode.c
	json_parse_array.c
	json_parse_object.c
	json_parse_string.c
	json_parse_value.c
	json_skipws.c
	json_value_clear.c json_encode_value.c json_encode_string.c json_encode_object.c json_encode_array.c)

