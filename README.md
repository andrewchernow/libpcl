
# OVERVIEW

The Portable C Library (PCL) is a library of cross-platform C code. It is designed for use on 
Microsoft Windows, Mac OS X and Linux x86/x86_64 platforms.

To view the documentation, please visit the [PCL API Documentation](https://andrewchernow.github.io/libpcl/).

### OpenSSL Instructions
PCL does not distribute any resources for OpenSSL. For Unix machines, openssl is almost 
always installed. You may need to install the development packge. For windows, there are
instructions provided below on how to install it.

Note for Unix machines, make sure that openssl header files are discoverable by GCC/Clang for 
building PCL. Also ensure that the shared objects can be found at link and runtime.

The best way to install OpenSSL on Windows for building and runtime, is to install the prebuilt
OpenSSL Windows package. This can be found here https://slproweb.com/products/Win32OpenSSL.html.
You want to install the "Win64 OpenSSL vX.X.X", where version is the current one listed. The
description makes it clear that this is for development: "Recommended for software developers by 
the creators of OpenSSL". If you want to build 32-bit, there is an installer listed there as
well. Do not install the "light" version!

This will install OpenSSL at C:\Program Files\OpenSSL-Win64. You will need to adjust your
Environment Variables so cl.exe can find the headers and and link.exe can find the import 
libraries. In addition, you also want DLLs to be found at runtime. 

#### OpenSSL Environment Variables
To add environment variables so the windows compiler and linker can find development resources
and the DLLs can be found at runtime, follow the below steps:

 * From Start Search, type "env" and you should see "Edit the system environment variables"
 * Click the "Environment Variables" button
 * Under "User variables" add or append the below (they may or may not exist)
   * INCLUDE -> C:\Program Files\OpenSSL-Win64\include (compile)
   * LIB -> C:\Program Files\OpenSSL-Win64\lib (link)
   * PATH -> C:\Program Files\OpenSSL-Win64 (runtime)

#### OpenSSL Packaging for Distribution
PCL provides no instructions on how to distribute OpenSSL DLLs with your application. That is
completely application-specific. PCL suggests installing the openssl DLLs, found in 
C:\Program Files\OpenSSL-Win64 directory, alongside your application EXE. Windows always 
searches for DLLs in the same directry as the executable file. Where ever your application
installs them, make sure the DLLs can be found at runtime.

### LICENSE
The Portable C Library ("PCL") is released under the Modified BSD License. Please refer to the
LICENSE file in the root of the PCL project. The license can also be found at the top of every 
source file within the PCL project.


### CONTRIBUTORS

Andrew Chernow <achernow@gmail.com>


### AUTHORS

Andrew Chernow <achernow@gmail.com>




<!--
Some note worthy features are:

  * Logging System
    * log event severity levels: debug, info, warn, error, fatal, panic
    * per thread management and facilities
    * specify which events are ignored or display stack trace at runtime

  * Full Unicode Support
    * Portable Character - \c pchar_t maps to a \c wchar_t UTF-16 on Windows and 
      a \c char UTF-8 on Unix
    * Extensive pchar_t API (using 'pcs' prefix): pcl_pcscmp, pcl_pcstrim, etc...
    * printf format specifier %Ps, %Pc for printing pchar_t characters or strings. The 'P' is the 
      length modifier (portable) and the 's' or 'c' is the conversion. Any PCL function that 
      takes a format specifier string, supports %Ps and %Pc.
    

  * Error Management
    * Unified error codes, maps OS codes to PCL codes
    * Unified error strings
    * error contexts that can be saved, swapped in or out, etc...
    * Full stack traces: print to buffer or stream. Stack traces are built manually (opt-in). 
      If a function fails and it is known to have set the PCL error, one simply calls
      a PCL trace function/macro. `if(call() < 0) return TRC();` this is a typical 
      use case. It means we know `call()` already set the PCL error and we opt-in to adding
      a trace. All SETERR* and TRC* macros return -1 by default. There are also R_SETERR*
      and R_TRC* macros for returning something other than -1: R_TRC(NULL) for example. 

  * Process Management
    * pcl_proc_exec: `execvp` or `CreateProcess*`, set process owner, STD
      stream redirection, asynchronous, set library path for dynamically
      linked object, etc...
    * pcl_popen: perl like syntax "< ls -l" which reads from STDOUT, "^ ls -l"
      which reads from STDERR or "> ls -l" which writes to STDIN. NOTE: MS
      CRT popen doesn't work unless its a command line application, while PCL's
      popen works in any environment.

  * Time Functions
    * nanosecond times: `pcl_time_t` with a 'sec' and 'nsec' member.
    * high-res timers: `pcl_clock()`

  * Thread/Synchronization
    * notifications of thread start and exit
    * Full TLS support
    * Mutex support

  * Collections
    * vector, hash table (dynamic rehash to shrink or grow), dynamic string buffer, etc.

  * System Information
    * CPU, Storage, Memory, Process, OS version, etc...

  * File and Directory Management
    * asynchronous I/O
    * MS Windows ACL support (SECURITY_DESCRIPTOR)
    * uid_t and gid_t implemented as SID strings on MS Windows
    * Unified struct stat called 'pcl_stat_t'
    * creation time (birth time) is included for stats 

  * Sockets & SSL/TLS
    * SSL/TLS backed by OpenSSL
    * supports 2-way certificate authentication
    * Socket API supports AF_INET, AF_INET6 and AF_UNIX SOCK_STREAMs

  * Memory Management
    * allocation failure callback
    * safe allocate/free functions (suffixed with '_s')


-->
