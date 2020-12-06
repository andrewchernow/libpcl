@mainpage Overview
This is the Portable C Library (PCL) API documentation. PCL is broken up into many different
modules, where each module's interface is within a header file. The best way to go through
this documentation, is to explore the API on a module level.

The best place to start is the "Related Pages" tab. Then navigate to each module by clicking 
on the "Modules" tab.

The Portable C Library (PCL) is a library of cross-platform C code which has been tested on the
following platforms:

* Microsoft Windows 7/2008+ (x86, x86_64, Itanium)
* Linux Kernel 2.6+         (x86, x86_64)
* Mac OS X 10.7+ (Darwin)   (x86, x86_64)

PCL began in ~2000 as a small utility library. Over the years, more functionality has been added
and the project continued to mature. Below are some of the features of PCL:

* Unified character handling via pchar_t: UTF-16 on windows and UTF-8 on Unixes
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
* Memory management with allocation failure callback


### LICENSE
The Portable C Library ("PCL") is released under the Modified BSD License. Please refer to the
LICENSE file in the root of the PCL project. The license can also be found at the top of every
source file within the PCL project.

### CONTRIBUTORS
Andrew Chernow <achernow@gmail.com>

### AUTHORS
Andrew Chernow <achernow@gmail.com>
