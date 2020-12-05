# POSIX threads For Windows
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

# Google's FarmHash
Google's FarmHash is distributed with PCL. The library is prebuilt, simply an object file.
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
