
add_library(json OBJECT
	json_arr.c
	json_arradd.c
	json_arraddbool.c
	json_arraddint.c
	json_arraddnull.c
	json_arraddreal.c
	json_arraddstr.c
	json_arrget.c
	json_arrgetint.c
	json_arrgetreal.c
	json_arrgetstr.c
	json_arrisarr.c
	json_arrisbool.c
	json_arrisint.c
	json_arrisnull.c
	json_arrisobj.c
	json_arrisreal.c
	json_arrisstr.c
	json_arrremove.c
	json_bool.c
	json_compile.c
	json_count.c
	json_decode.c
	json_encode.c
	json_encode_array.c
	json_encode_object.c
	json_encode_string.c
	json_encode_value.c
	json_false.c
	json_free.c
	json_freepath.c
	json_int.c
	json_match.c
	json_null.c
	json_obj.c
	json_objget.c
	json_objgetint.c
	json_objgetreal.c
	json_objgetstr.c
	json_objisarr.c
	json_objisbool.c
	json_objisint.c
	json_objisnull.c
	json_objisobj.c
	json_objisreal.c
	json_objisstr.c
	json_objput.c
	json_objputbool.c
	json_objputint.c
	json_objputnull.c
	json_objputreal.c
	json_objputstr.c
	json_objremove.c
	json_parse_array.c
	json_parse_number.c
	json_parse_object.c
	json_parse_string.c
	json_parse_value.c
	json_real.c
	json_skipws.c
	json_str.c
	json_strn.c
	json_true.c
	json_objistrue.c json_objisfalse.c json_arristrue.c json_arrisfalse.c json_query.c)

