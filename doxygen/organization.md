# Library Organization {#org}
@brief Overview of library organization.

The PCL library is organized into modules. Each module has its own header file and source directory 
of the same name. Every function within a module is organized into separate source files. Some
modules define a private header, typically _modulename.h, that is used to declare opaque 
structures or internal functions required to implement the public interface. 

## Doxygen Directory
The doxygen directory contain resource pulled into the generated documentation: markdown, html, 
etc. files. The generated documents are stored in a docs directory on a "docs" branch. The
Doxyfile configuration file is in the root of the repository.

## Examples Directory
This directory contains example programs for different PCL modules. This is a great place to 
quickly learn how to use PCL; and how easy it is to use.

## Include Directory
This contains the "pcl" directory. Every header file is within the pcl directory. This allows
installation of pcl header files within a system without running into name conflicts. For
eaxmple, installing into \c /usr/include would place all headers into \c /usr/include/pcl.
When including PCL headers, use `#include <pcl/header.h>`. Any header file that begins with
an underscroe \c _, should not be included directly.

## Libs Directory
This contains 3rd party libraries required for PCL to function properly. This is organized
into platform folders that all contain and \c include directory. For example:

  * darwin-x86
  * windows-x86_64
  * linux-x86

Third-party objects, archives or shared objects are placed in the platform-specific lib directory
while the associated headers go into the include directory.  

### Google FarmHash
This is the only library used by all platforms. Each lib directory contains a pre-compiled object
file and a \c farmhash.h file in the include directory. This is only used by the PCL hash table.

To rebuild or build differently, follow the below instructions.

For any platform, clone https://github.com/google/farmhash and `cd` into the src directory of
the project.

#### Windows
For windows, open the x64 or x86 Visual Studio prompt and run the below:
```
cl /nologo /MD /wd4267 /wd4244 /wd4319 /O2 /W3 /DFARMHASH_NO_BUILTIN_EXPECT -c farmhash.cc
```

Copy farmhash.h to `libs\windows-x86|x86_64\include` and farmhash.obj to
`libs\windows-x86|x86_64`.

#### Darwin & Linux
For Darwin, add `-mmacosx-version-min=10.9` to the below. For 32-bit builds, add -m32 to the below.

```
gcc -Wall -g0 -mavx -maes -O3 -o farmhash.o -c farmhash.cc
```

Copy farmhash.h to `libs\linux|darwin-x86|x86_64\include` and farmhash.obj to
`libs\linux|darwin-x86|x86_64`.

#### POSIX Threads For Windows
PCL includes prebuilt binaries for POSIX threads for Windows. To update or build this
differently, use the below instructions.

To build the included pthreads library for windows, you need to download the source here:
https://sourceforge.net/projects/pthreads4w/. Goto the "Files" tab and select the v3 zip
file. After unzipping, open a x64 Visual Studio command prompt.
```
nmake clean VC
```

That will produce a pthreadVC3.lib and pthreadVC3.dll that needs to be copied to
`libs\windows-x86_64` directory. Then copy pthread.h, semaphore.h, sched.h and _ptw32.h
to `libs\windows-x86_64\include`

Follow the above instructions for x86. Open x86 Visual Studio command prompt. Copy the same
files to `libs\windows-x86` and `libs\windows-x86\include`.

## Src Directory
The \c src directory contains one sub-directory per module. Each sub-directory contains one file 
per module function, with a CMakeLists.txt file.
