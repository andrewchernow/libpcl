
# OVERVIEW

The Portable C Library (PCL) is a library of cross-platform C code,
which has been tested on the following platforms:

  * Microsoft Windows 7/2008+ (x86, x86_64, Itanium)
  * Linux Kernel 2.6+         (x86, x86_64, MIPS, ARM)
  * Mac OS X 10.4+ (Darwin)   (x86, x86_64, PowerPC)

PCL may support other Operating Systems in the future, but at this point it
is focused on the three listed above. 

Some note worthy features are:

  * Logging System
    * log event severity levels: debug, info, warn, error, fatal, panic
    * per thread management and facilities
    * specify which events are ignored or display stack trace at runtime

  * Full Unicode Support
    * tchar_t: 'wchar_t' UTF-16 on Microsoft Windows and 'char' UTF-8 on Unixes.
    * extensive tchar_t API (using 'tcs' prefix): pcl_tcscmp, pcl_tcstrim, etc...
    * printf specifier %ts, %Ts, %tc, %Tc for printing tchar_t characters
      or strings: via `pcl_printf` and friends.

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


LICENSE:

The Portable C Library (PCL) is released under the Modified BSD License.


CONTACTS/CONTRIBUTORS:

Andrew Chernow <achernow@gmail.com>


AUTHORS:

Andrew Chernow
