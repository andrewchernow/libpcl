
# OVERVIEW

The Portable C Library (PCL) is a library of cross-platform C code,
which has been tested on the following platforms:

  * Microsoft Windows 7/2008+ (x86, x86_64, Itanium)
  * Linux Kernel 2.6+         (x86, x86_64, MIPS, ARM)
  * Mac OS X 10.4+ (Darwin)   (x86, x86_64, PowerPC)

PCL may support other Operating Systems in the future, but at this point it
is focused on the three listed above. For example, most of PCL will run on
FreeBSD, due to similarities with Linux and Darwin, but no porting effort
has commenced.

PCL offers many features, too many to list here, but the highlights are
listed below. The below just scratches the surface. Keep in mind, PCL
is 100% pure C, there is no C++. There are some pending features that
require C++ APIs on MS Windows, but those will be exposed as C API wrappers.

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
    * Full backtracing: print to buffer or stream, JSON output, full mgmt.

  * Process Management
    * pcl_proc_exec: `execvp` or `CreateProcess*`, set process owner, STD
      stream redirection, asynchronous, set library path for dynamically
      linked object, etc...
    * pcl_popen: perl like syntax "< ls -l" which reads from STDOUT, "^ ls -l"
      which reads from STDERR or "> ls -l" which writes to STDIN. NOTE: MS
      CRT popen doesn't work unless its a command line application: no
      WinMain/Service/Daemon support. The pcl_popen works in any environment.

  * Time Functions
    * nanosecond times: pcl_time_t with a 'sec' and 'nsec' member.
    * high-res timers

  * Thread/Synchronization
    * notifications of thread start and exit
    * Full TLS support

  * Collections
    * vector, stack, hash table, etc...

  * System Information
    * CPU, Storage, Memory, Process, OS version, etc...

  * File and Directory Management
    * asynchronous I/O
    * MS Windows ACL support (SECURITY_DESCRIPTOR)
    * uid_t and gid_t implemented as SID strings on MS Windows
    * Unified struct stat called 'pcl_stat_t'

  * Sockets & SSL/TLS
    * SSL/TLS backed by OpenSSL
    * supports 2-way certificate authentication

  * Memory Management
    * allocation failure callback
    * safe allocate/free functions (suffixed with '_s')


LICENSE:

The Portable C Library (PCL) is released under the Modified BSD License.
Please see the LICENSE file included within the root of this project.


CONTACTS/CONTRIBUTORS:

Andrew Chernow <achernow@gmail.com>


AUTHORS:

Andrew Chernow
