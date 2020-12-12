# Error System {#errsys}
@brief Error code mapping, stack traces and formatted error output.

The PCL error system provides a suite of functions in the \ref error "error" module. 
A primary design goal of PCL is to provide a robust error system.

The error system operates per-thread. Thread local storage (TLS) is used to maintain a per-thread
::pcl_err_t. So there is no global error. Each thread is unique. To get the per-thread
error, use the ::pcl_err_get function.

## Error Codes
PCL provides ~140 error codes. There are internal mapping tables to map Operating System error 
codes (oserr) to PCL. On unix systems, these mapping tables include all error codes defined in 
\c errno.h. On Windows, ~350 Win32 codes are mapped. In addition, there are also mappings 
for Windows CRT (POSIX errno.h support) to Win32.

All error code names begin with \c PCL_E. Most errors use the same naming as one would find in
\c errno.h: \c PCL_EINVAL, \c PCL_ENOENT, \c PCL_ERANGE, \c PCL_ENETRESET, etc.

There are several public functions that leverage these mapping tables:

  * ::pcl_err_msg - looks up a PCL error code and returns the default error message
  * ::pcl_err_name - looks up a PCL error code and returns its name: \c "PCL_EISDIR"
  * ::pcl_err_code - looks up a PCL error name and returns the PCL error code  
  * ::pcl_err_os2pcl - looks up an OS error code and returns the PCL code
  * ::pcl_err_osname - looks up an OS error code and returns its name: like Win32 
    \c "ERROR_FILE_NOT_FOUND" or Unix \c "ENOENT"
  * ::pcl_err_osmsg - looks up an OS error code and returns the default OS error message
  * ::pcl_err_crt2pcl - For Windows, takes a CRT error code, like \c EINTR, and returns the PCL 
    code, like \c PCL_EINTR. On Unix, the CRT error code is identical to the OS error code
  * ::pcl_err_crt2os - For Windows, takes a CRT error code, like \c EEXIST, and returns the OS 
    error code, like \c ERROR_ALREADY_EXISTS. On Unix, the CRT error code is identical to the 
    OS error code

## Setting Errors
Setting PCL errors typically involves using one of many macros. These macros all wrap the
::pcl_err_set function, which internally uses the ::pcl_err_vset function. These set functions
are almost never used directly: they take many arguments including \__FILE__, \__func__, \__LINE__,
pcl error, os error, format string and variable arguments. The error set macros make setting 
an error easy. The set error macros all begin with \c SETERR:

  * ::SETERR - sets the pcl error code
  * ::SETERR_OS - same as \c SETERR but also sets OS error code
  * ::SETERRMSG - sets the pcl error code and a formatted message
  * ::SETERRMSG_OS - same as \c SETERRMSG but also sets OS error code
  * ::SETOSERR - sets the os error code, internally mapped to PCL error code
  * ::SETOSERRMSG - same as \c SETOSMSG but includes formatted message
  * ::SETLASTERR - very common, sets the last OS error code. The os error code is
    the current Unix \c errno or for Windows, \c GetLastError, _doserrno or CRT \c errno. 
    For any platform, you can use ::pcl_oserrno to get the last OS error 
  * ::SETLASTERRMSG - same as \c SETLASTERR but includes a formatted message   

All \c SETERR* macros expand to -1. If the error set is ::PCL_EOKAY (0), they expand to zero (almost
never happens). By expanding to -1, \c SETERR* macros can be used with a \c return statement. This
is by design, since most PCL functions return -1 to indicate an error.
```c
int some_func(const char *str)
{
  if(str == NULL)
    return SETERR(PCL_EINVAL); // can also use BADARG() macro
  return (int) strlen(str);
}
```
As common as a -1 return value is to indicate an error, this is not always the case. A function
may return any number of values or data types to indicate an error. For this reason, every 
\c SETERR* macro has a cooresponding \c R_SETERR* macro. These macros allowing setting the 
return value as the first argument to something other than -1.
```c
char *some_func(const char *str)
{
  if(str == NULL)
    return R_SETERR(NULL, PCL_EINVAL); 
  return strdup(str);
}
```
In the above case, we are not only setting the error but also indicating what value the macro
should expand to -- which is designed to be the return value; \c R_ for return. 

There are cases where after detecting an error, a set of cleanup functions need to be executed.
In some cases, those cleanup functions may set the OS error code or even the PCL error code.
The problem is that setting the error and returning may be the wrong error. To solve this
issue, you can freeze the PCL error system using the ::FREEZE_ERR macro:
```c
pcl_file_t *some_func(pcl_file_t *file)
{
  // another_func uses a SETERR* macro	
  if(another_func(file) < 0)
  {
  	// preserve error set by another_func
    FREEZE_ERR
    (
      pcl_file_seek(file, 0, SEEK_SET); // could set PCL error 
      etc..
    );
    
    return NULL;
  }

  return file;
}
```

## Stack Trace
PCL provides stack trace support, implemented as a linked list stack. When an error is set,
PCL also adds a trace to the stack. There are two functions for adding to the current stack 
trace: ::pcl_err_trace and ::pcl_err_vtrace. It is uncommon to use these functions directly
for the same reason as the pcl error set functions: they are a pain to call directly. For this
reason, there are four macros for adding a trace item to the stack: ::TRC, ::TRCMSG, ::R_TRC and
::R_TRCMSG. The \c TRC macro takes no arguments and expands to -1. \c TRCMSG is identical to
\c TRC but allows setting a formatted trace message; great for adding context to the stack trace.
The \c R_TRC and \c R_TRCMSG macros are identical to the non-R versions but just like \c R_SETERR*
macros, take the expand/return value as the first argument.

In the previous example, ::FREEZE_ERR was used to avoid cleanup functions from overwriting the
actual error. One thing that example failed to do, was add a trace. It can be rewritten as:
```c
pcl_file_t *some_func(pcl_file_t *file)
{
  // another_func uses a SETERR* macro	
  if(another_func(file) < 0)
  {
  	// preserve error set by another_func
    FREEZE_ERR
    (
      pcl_file_seek(file, 0, SEEK_SET); // could set PCL error 
      etc..
    );
    
    return R_TRC(NULL); // add to stack trace
  }

  return file;
}
```
This is very useful, especially if \c another_func is called from many different places. Adding
a trace via \c R_TRC(NULL), tells us exactly how we got to the error call site. 

Stack tracing is obviously opt-in. If you don't use the tracing macros, then you will only get
the error call site trace information. A rule of thumb is, if the function you are calling 
is known to set the PCL error, then you want to set a trace. If the function, such as a function
outside the PCL library, doesn't set the PCL error than use a \c SETERR* macro to begin the 
stack trace. So, tracing is not only opt-in but it is important to know what, if any, error
handling was implemented by the function that failed. This is normally pretty straightforward.

## Formatted Output
There are several functions for formatting an error. They are designed to output a
formatted stack trace. 

  * ::pcl_err_fprintf - outputs a stack trace to a given FILE stream
  * ::pcl_err_sprintf - outputs a stack trace to a given buffer

Each of the above functions also have \c va_list versions: \c vfprintf and \c vsprintf. The 
\c FILE stream versions prepare the stack trace in memory and issue a single \c fwrite when 
complete.

All take an \a indent as an argument. Normally, \a indent is set to zero. Each trace is written
to its own line starting at indent and increments by 2 every line. Below is an example
stack trace from examples/ls.c. That example takes a directory as an argument and performs an
`ls -l` style listing. If the example cannot open the given directory, it issues a panic: 
```c
51  pcl_dir_t *dp = pcl_opendir(argv[1]);
52
53  if(!dp)
54    PANIC("Failed to open directory '%Ps'", argv[1])
```
::PANIC performs a \c TRC, outputs a stack trace to \c stderr and exits. To avoid the exit, use 
the ::PTRACE macro instead. 

Below is a sample output of running `./ex_ls file.txt`
```
1  PANIC: Failed to open directory 'file.txt'
2  /Users/andrew/projects/libpcl/examples/ls.c:main(54)
3    /Users/andrew/projects/libpcl/src/dir/opendir.c:pcl_opendir(90): PCL_ENOTDIR - Not a directory.
4      >Cannot open directory: file.txt
```
@note line numbers plus two spaces are added for example purposes, not part of normal output. 

__Line #1__ is the optional message passed to \c PANIC or \c pcl_err_fprintf. If the \a indent 
parameter to \c pcl_err_fprintf and friends is set to \c -1, then this line is prefixed 
with \c "PANIC: " rather then \c "ERROR: ". \c PANIC always sets this to \c -1. Note that 
\c PTRACE does not set \a indent to \c -1. This line is not counted as part of the indent. 
Meaning, after this line is outputed the indent is not incremented by 2 spaces.

__Line #2__ represents the top of the stack trace. This is the last trace added and represents 
the initial call site that caused the error at the bottom of the stack. By looking at this line, 
we can tell that either \c TRC or \c R_TRC was used as there is no context message. Below is
an example of what line #2 would look like if `TRCMSG("context message")` or 
`R_TRCMSG(NULL, "context message")` were used:
```c
2  /Users/andrew/projects/libpcl/examples/ls.c:main(54) - context message
```
Both \c TRCMSG and \c R_TRCMSG take a format and variable arguments.

__Line #3__ is the bottom of the stack, the error call site. This is where the error was 
initially set via \c SETERR* or \c SETLASTERR*. Keep in mind, that there is typically more
lines of tracing in a real application. This line always includes the error name, \c PCL_ENOTDIR,
and the default error message for that PCL error. OS errors, if captured, are never displayed
in PCL generated stack traces.

__Line #4__ is the application-specific error message provided when setting the error. This is
optional so it is very possible to find stack traces without the ">application message" line.
Between setting context messages while adding traces and setting an application error message
when setting the error, you end up with a wealth of information before examining a single line 
of code.

You can also format your own stack trace by getting a pointer to the current thread's error
via ::pcl_err_get(). The structure contains the PCL and OS error code. It also contains the top 
of the stack trace: pcl_err_t.strace. The OS error code is only set if it was provided during 
a \c SETERR* macro, or the \c SETLASTERR* macros. It may very well be zero, which indicates that
a PCL error code was directly set, verse first mapping an OS error code to PCL as \c SETLASTERR
does.
