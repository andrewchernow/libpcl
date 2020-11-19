
# OVERVIEW

The Portable C Library (PCL) is a library of cross-platform C code which has been tested on the 
following platforms:

  * Microsoft Windows 7/2008+ (x86, x86_64, Itanium)
  * Linux Kernel 2.6+         (x86, x86_64)
  * Mac OS X 10.7+ (Darwin)   (x86, x86_64)

PCL began in ~2000 as a small utility library. Over the years, more functionality has been added
and the project continued to mature. Below are some of the features of PCL:

  * Unified character handling via tchar_t: UTF-16 on windows and UTF-8 on Unixes
  * Suite of safe string functions: unlike strcpy or sprintf
  * Robust error management: stack trace, PCL error code, per thread error context
  * Process management: exec, popen, process path, etc.
  * Asyncronous I/O: sockets, files, pipes, etc. 
  * Unified file and directory management: via pcl_file_t object (portable 'fd')  
  * Consistent time management: epoch nanosecond support, nanosecond file times, etc.
  * Detailed system information  
  * POSIX feel: mimic many function and header names
  * Thread safe and reentrant
  * File security: Win32 SECURITY_DESCRIPTOR mapped to Unix mode and uid/gid SID strings  
  * Vanilla and SSL sockets: SOCK_STREAM for AF_INET, AF_INET6, AF_UNIX support only   
  * Basic data structures: hash table, vector, dynamic string buffer, etc.
  * Memory management (settable allocation failure callabck)

PCL is broken into smaller modules. Each module lives in its own directory. Within each directory
is a README.md describing the module specifics with examples. All header files are within a
"pcl" directory and are safe to install into your system's include path: ex. `<pcl/file.h>`

## Initialize PCL
PCL must be initialized before use: via `pcl_init`. This is typically called within `main`. 
It is recommended to use PCL's pcl_tmain function which maps to `wmain` on Windows and
`main` on unixes.

If an application wants to receive the `PCL_EVENT_INIT` event, it must populate a `pcl_init_t`
structure and pass it to `pcl_init`. An application would want the `PCL_EVENT_INIT` event if
it has to perform intialization before any other code is executed: like creating a mutex
or some TLS storage. See [PCL Events](/andrewchernow/libpcl/tree/master/src/event/README.md) 
for more information.

```c
// First Example: demonstrates utilizing pcl_init_t

#include <pcl/init.h>
#include <pcl/event.h>
#include <pcl/tchar.h>

static void my_register_handlers(pcl_init_t *init)
{
  // If the "some-feature" option is provided, some_feature_handler 
  // will receive a PCL_EVENT_INIT event
  if(pcl_tcscmp(init->argv[1], _T("some-feature")) == 0)
    pcl_event_register(some_feature_handler);
  else if (...blah...)
    pcl_event_register(another_feature_handler);
  
  pcl_event_register(always_register_handler);
}

int pcl_tmain(int argc, tchar_t **argv)
{
  pcl_init_t init = {
    .argc = argc,
    .argv = argv, // tchar_t *
    .userp = NULL,
    .register_handlers = my_register_handlers
  };

  pcl_init(&init);
  app_start();
}

// 2nd Example: no pcl_init_t and manually register handlers

#include <pcl/init.h>
#include <pcl/tchar.h>

int pcl_main(int argc, tchar_t **argv)
{		
  pcl_init(NULL);
  pcl_event_register(some_func, NULL); // never gets a PCL_EVENT_INIT
  pcl_event_disptach(PCL_EVENT_POSTINIT, NULL); // some_func gets this event
  app_start();
}
```

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
