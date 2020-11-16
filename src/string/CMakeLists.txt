
add_library(string OBJECT
	astrcatf.c
	astrrep.c
	strcatf.c
	strfbytes.c
	strhexdec.c
	strhexenc.c
	strlower.c
	strncat.c
	strncpy.c
	strndup.c
	strrep.c
	strsignal.c
	strsplit.c
	strtrimset.c
	strtrunc.c
	strupper.c)

# needed by darwin and windows
if(NOT LINUX)
	target_sources(string PRIVATE stristr.c)
endif()
