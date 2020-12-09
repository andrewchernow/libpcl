# Error System {#errsys}
@brief Error code mapping, stack traces and formatted error output.

The PCL error system provides a suite of functions across two modules: \ref error "error" and 
\ref errctx "errctx". A primary design goal of PCL is to provide a robust error system.

The error system operates per-thread. Thread local storage (TLS) is used to maintain a per-thread
::pcl_err_ctx_t. So there is no global error context. Each thread is unique. To get the per-thread
conetxt, use the ::pcl_err_ctx function.

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


