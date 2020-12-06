# Unicode Support {#unicode}
@brief Unicode support through portable character strings.

A Portable Character, or Portable Character String, is a platform-specific character for character
and string management on a specific platform. On Windows, a \c pchar_t is always a \c wchar_t 
which is encoded as UTF-16. On Unix machines, a \c pchar_t is a \c char encoded as UTF-8. The 
portable character exists so applications can be unicode on either Windows or Unix. It is very
important to interact with the Win32 API with \c UNICODE defined. This causes windows to use
the unicode versions (wide \c wchar_t) of many functions and/or macros. On Unix, there are no
special macros or named functions to support unicode since UTF-8 is used. 

#### Portable Character Naming
The naming convention for portable character functions is consistent throughout the PCL library.
PCL uses \c pcs for portable character string, mimicing the standard \c wcs for wide character
string. In addition, the letter \c p is used where one would find a \c w for \c wchar_t
functions. All PCL functions or macros begin with \c pcl_ or \c PCL_, creating a namespace.

char character | wchar_t wide character | pchar_t portable character
----------- |------------|-------------
strcmp |  wcscmp | pcl_pcscmp
tolower | towlower | pcl_toplower
sprintf | swprintf | pcl_spprintf
memcmp | wmemcmp | pcl_pmemcmp

Almost all of the string functions within PCL provide a "str" and "wcs" version, and then macros
are used to map them to the "pcs" version. Many are documented as functions to make it easier
to fully document parameters and return values. In these cases, a note indicates that it is
actually implemented as a macro.

#### Formatted Ouput Specifiers
PCL offers three extensions to the standard printf family: \c \%Ps for portable character strings, 
\c \%Pc for portable characters and \c %/ for the platform-specific path separator. 
The \c P is the length modifier while the \c s or \c c is the conversion specifier.

Below is an example of using portable character strings with PCL. Assume 
this is ran at a BASH or DOS prompt:
```bash
# "file name" in chinese
shell]# ./print_sizes "文件名"
```
For starters, an application needs to be able to accept UTF-8 or UTF-16 arguments. PCL offers 
\c pcl_main, which is \c main on Unix and \c wmain on Windows:
```c
/*
 * print_sizes.c - Output the size of each UTF-8 encoded newline separated 
 * pathname and a total size when complete.
 */

#include <pcl/init.h>
#include <pcl/io.h>
#include <pcl/stat.h>
#include <pcl/limits.h>

int pcl_main(int argc, pchar_t **argv)
{
  pcl_init();

  // use 'b' for binary on Windows, no effect on Unix
  FILE *fp = pcl_fopen(argv[1], _P("rb")); 
  
  char utf8path[PCL_MAXPATH]; 
  uint64_t total = 0;
  
  while(fgets(utf8path, sizeof(utf8path), fp))
  {
  	// pcl_stat expects a pchar_t string so convert the UTF-8 path to a 
  	// portable character string. On Unix, this is a fancy strdup since 
  	// the portable encoding is UTF-8.
    pchar_t *path = pcl_utf8_to_pcs(pcl_strrtrim(buf), 0, NULL);

    pcl_stat_t st;

    if(pcl_stat(path, &st) == 0)
    {
      total += st.size;
      pcl_printf("%Ps - %lld\n", path, st.size);
    }
    
    pcl_free(path);
  }
  
  fclose(fp);
  
  printf("Total: %lld\n", total);
  
  return 0;
}
```
First, this example uses ::pcl_fopen which takes a \c pchar_t file and mode. Underneath the
scenes, ::pcl_fopen uses \c _wfopen_s on Windows and \c fopen on Unix. Thus, the file and 
mode arguments can be passed straight through to the platform-specific functions without having
to translate them.

After reading each line, the UTF-8 string is converted to a portable character string that is then
passed to ::pcl_stat. As stated above, there is no conversion when on Unix since a Unix portable
character string is already UTF-8. 

If the \c pcl_stat succeeds, the file is included in the formatted output. This demostrates 
using a PCL formatted output extension: \c \%Ps.

When looking at the above code, all of these dirty details are abstracted away, allowing an 
application to focus on more important things.


