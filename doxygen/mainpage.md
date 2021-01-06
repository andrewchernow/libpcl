@mainpage Overview
This is the Portable C Library (PCL) documentation. It includes API documentation, examples 
and a detailed overview of PCL concepts.

It is recommended to read through the PCL overview documents before proceeding to the
API documentation. The below documents can also be found in the "Related Pages" tab.

  * \ref org
  * \ref unicode
  * \ref openssl
  * \ref errsys

After reading through the overview documents, navigate to the "Modules" tab. This is a great way
to go through the API documentation.

The GitHUb repository is here: https://github.com/andrewchernow/libpcl .

The Portable C Library (PCL) is a library of cross-platform C code which has been tested on the
following platforms:

* Microsoft Windows 7/2008+ (x86, x86_64, Itanium)
* Linux Kernel 2.6+         (x86, x86_64)
* Mac OS X 10.7+ (Darwin)   (x86, x86_64)

PCL began in ~2000 as a small utility library. Over the years, more functionality has been added
and the project continued to mature. Below are some of the features of PCL:

* Unified character handling via pchar_t: UTF-16 on windows and UTF-8 on Unixes
* Complete JSON encode and decode library  
* Suite of safe string functions: unlike strcpy or sprintf
* Robust error management: stack trace, PCL error code, per thread error
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
