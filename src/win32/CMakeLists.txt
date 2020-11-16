
add_library(win32 OBJECT
	win32_file_security.c
	win32_logon.c
	win32_mode_to_rights.c
	win32_name_to_sid.c
	win32_process_sid.c
	win32_process_token.c
	win32_rights_to_mode.c
	win32_sd_clear.c
	win32_sd_create.c
	win32_sd_get.c
	win32_sd_init.c
	win32_sid_dup.c
	win32_sid_to_name.c
	win32_token_info.c
	win32_world_sid.c)
