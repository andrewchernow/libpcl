/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
  All rights reserved.

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  * Redistributions of source code must retain the above copyright notice, this
    list of conditions and the following disclaimer.

  * Redistributions in binary form must reproduce the above copyright notice,
    this list of conditions and the following disclaimer in the documentation
    and/or other materials provided with the distribution.

  * Neither the name of the copyright holder nor the names of its
    contributors may be used to endorse or promote products derived from
    this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#ifndef LIBPCL__ERRNO_H
#define LIBPCL__ERRNO_H

/** @addtogroup errno
 * @{
 */

#include <errno.h>

/** The operation was successful. */
#define PCL_EOKAY                             0
/** Arg list too long. */
#define PCL_E2BIG                             1
/** Access is denied. */
#define PCL_EACCES                            2
/** Try again. */
#define PCL_EAGAIN                            3
/** Operation is now in progress. */
#define PCL_EINPROGRESS                       4
/** Operation is already in progress */
#define PCL_EALREADY                          5
/** Bad file descriptor. */
#define PCL_EBADF                             6
/** Invalid request. */
#define PCL_EBADCMD                           7
/** Device or resource busy. */
#define PCL_EBUSY                             8
/** Operation cancelled. */
#define PCL_ECANCELLED                        9
/** No child processes. */
#define PCL_ECHILD                            10
/** Resource deadlock would occur. */
#define PCL_EDEADLK                           11
/** Disc quota exceeded. */
#define PCL_EDQUOT                            12
/** Object or resource already exists. */
#define PCL_EEXIST                            13
/** Bad Address. */
#define PCL_EFAULT                            14
/** File too large. */
#define PCL_EFBIG                             15
/** Invalid or incomplete multibyte or wide character. */
#define PCL_EILSEQ                            16
/** Interrupted system call. */
#define PCL_EINTR                             17
/** Invalid argument. */
#define PCL_EINVAL                            18
/** I/O error. */
#define PCL_EIO                               19
/** Is a directory. */
#define PCL_EISDIR                            20
/** Too many symbolic links. */
#define PCL_ELOOP                             21
/** Process open file limit has been exceeded. */
#define PCL_EMFILE                            22
/** Too many hard links to the given file or directory. */
#define PCL_EMLINK                            23
/** System open file limit has been exceeded. */
#define PCL_ENFILE                            24
/** No such device. */
#define PCL_ENODEV                            25
/** No such file or directory. */
#define PCL_ENOENT                            26
/** Exec format error. */
#define PCL_ENOEXEC                           27
/** No locks available. */
#define PCL_ENOLCK                            28
/** Out of memory. */
#define PCL_ENOMEM                            29
/** No such process. */
#define PCL_ESRCH                             30
/** No space left on device. */
#define PCL_ENOSPC                            31
/** Not a stream. */
#define PCL_ENOSTR                            32
/** Operation/Function not implemented. */
#define PCL_ENOIMPL                           33
/** Not a directory. */
#define PCL_ENOTDIR                           34
/** Directory is not empty. */
#define PCL_ENOTEMPTY                         35
/** Operation not supported. */
#define PCL_ENOTSUP                           36
/** Value too large to be stored in data type. */
#define PCL_EOVERFLOW                         37
/** Permission denied. */
#define PCL_EPERM                             38
/** Broken pipe. */
#define PCL_EPIPE                             39
/** Too many processes. */
#define PCL_EPROCLIM                          40
/** Math result not representable. */
#define PCL_ERANGE                            41
/** Read-only file system. */
#define PCL_EROFS                             42
/** Illegal seek. */
#define PCL_ESPIPE                            43
/** Try again. */
#define PCL_EWOULDBLOCK                       PCL_EAGAIN
/** Cross-device operation not permitted. */
#define PCL_EXDEV                             45
/** Name is too long. */
#define PCL_ENAMETOOLONG                      46
/** Invalid index. */
#define PCL_EINDEX                            47
/** Buffer too small. */
#define PCL_EBUF                              48
/** Invalid or corrupt format. */
#define PCL_EFORMAT                           49
/** Invalid type. */
#define PCL_ETYPE                             50
/** No more items. */
#define PCL_ENOMORE                           51
/** Device or resource is not ready or initialized. */
#define PCL_ENOTREADY                         52
/** Invalid pathname. */
#define PCL_EPATH                             53
/** Undefined error occurred. */
#define PCL_EUNDEF                            54
/** Object or resource not found. */
#define PCL_ENOTFOUND                         55
/** End of file.	*/
#define PCL_EOF                               56
/** Encryption or decryption failed. */
#define PCL_ECRYPT                            57
/** Operation timed out. */
#define PCL_ETIMEOUT                          58
/** Authentication error. */
#define PCL_EAUTH                             69
/** Version Conflict */
#define PCL_EVERSION                          70
/** Interrupted system call should be restarted */
#define PCL_ERESTART                          71
/** Link has been severed */
#define PCL_ENOLINK                           72
/** Device error. */
#define PCL_EDEVERR                           73
/** Attempting to link in too many shared libraries */
#define PCL_ELIBMAX                           74
/** Out of streams resources */
#define PCL_ENOSR                             75
/** Object or resource is unavailable */
#define PCL_EUNAVAIL                          76
/** Object or resource has expired */
#define PCL_EEXPIRED                          77
/** Object or resource has been rejected */
#define PCL_EREJECTED                         78
/** Object or resource has been revoked */
#define PCL_EREVOKED                          79
/** Object or resource has been disabled */
#define PCL_EDISABLED                         80
/** Out of sync */
#define PCL_ESYNC                             81
/** Database error */
#define PCL_EDB                               82
/** The resource is currently locked. */
#define PCL_ELOCK                             83
/** The operation has been aborted. */
#define PCL_EABORT                            84
/** Message digest failure. */
#define PCL_EDIGEST                           85
/** Security violation. */
#define PCL_ESECURITY                         86
/** Object or resource has moved. */
#define PCL_EMOVED                            87
/** Object is not an instance of class. */
#define PCL_ECLASS                            88
/** SSL error */
#define PCL_ESSL                              89
/** Out of sequence */
#define PCL_ESEQ                              90
/** Syntax error */
#define PCL_ESYNTAX                           91
/** Bad handle value */
#define PCL_EBADH                             92
/** Database query error */
#define PCL_EDBQUERY                          93

/** Start code for network error */
#define PCL_ERR_NETSTART 200
/** End code for network error */
#define PCL_ERR_NETSTOP 300

/* Indicates if a given error code is a network error. Excludes EAGAIN */
PCL_INLINE bool
pcl_err_isnet(int e)
{
	return (e >= PCL_ERR_NETSTART && e < PCL_ERR_NETSTOP) || e == PCL_EPIPE;
}

/** The internet address is already in use. */
#define PCL_EADDRINUSE                        (PCL_ERR_NETSTART + 0)
/** Can't assign the requested internet address. */
#define PCL_EADDRNOTAVAIL                     (PCL_ERR_NETSTART + 1)
/** Internet address family not supported by protocol family. */
#define PCL_EAFNOSUPPORT                      (PCL_ERR_NETSTART + 2)
/** Software caused connection abort. */
#define PCL_ECONNABORTED                      (PCL_ERR_NETSTART + 3)
/** Connection refused. */
#define PCL_ECONNREFUSED                      (PCL_ERR_NETSTART + 4)
/** Connection reset by peer. */
#define PCL_ECONNRESET                        (PCL_ERR_NETSTART + 5)
/** Destination internet address required. */
#define PCL_EDESTADDRREQ                      (PCL_ERR_NETSTART + 6)
/** Host is down. */
#define PCL_EHOSTDOWN                         (PCL_ERR_NETSTART + 7)
/** No route to host. */
#define PCL_EHOSTUNREACH                      (PCL_ERR_NETSTART + 8)
/** Socket is already connected. */
#define PCL_EISCONN                           (PCL_ERR_NETSTART + 9)
/** Message too long. */
#define PCL_EMSGSIZE                          (PCL_ERR_NETSTART + 10)
/** Hopping to multiple remote machines is prohibited. */
#define PCL_EMULTIHOP                         (PCL_ERR_NETSTART + 11)
/** Network is down. */
#define PCL_ENETDOWN                          (PCL_ERR_NETSTART + 12)
/** Network dropped connection on reset. */
#define PCL_ENETRESET                         (PCL_ERR_NETSTART + 13)
/** Network is unreachable. */
#define PCL_ENETUNREACH                       (PCL_ERR_NETSTART + 14)
/** No buffer space available. */
#define PCL_ENOBUFS                           (PCL_ERR_NETSTART + 15)
/** Protocol not available. */
#define PCL_ENOPROTOOPT                       (PCL_ERR_NETSTART + 16)
/** Socket is not connected. */
#define PCL_ENOTCONN                          (PCL_ERR_NETSTART + 17)
/** Socket operation was attempted on a non-socket. */
#define PCL_ENOTSOCK                          (PCL_ERR_NETSTART + 18)
/** Operation not supported. */
#define PCL_EOPNOTSUPP                        (PCL_ERR_NETSTART + 19)
/** Protocol family not supported. */
#define PCL_EPFNOSUPPORT                      (PCL_ERR_NETSTART + 20)
/** Protocol not supported. */
#define PCL_EPROTO                            (PCL_ERR_NETSTART + 21)
/** Protocol error. */
#define PCL_EPROTONOSUPPORT                   (PCL_ERR_NETSTART + 22)
/** Protocol wrong type for socket. */
#define PCL_EPROTOTYPE                        (PCL_ERR_NETSTART + 23)
/** Too many levels of remote in path. */
#define PCL_EREMOTE                           (PCL_ERR_NETSTART + 24)
/** Can't send after socket shutdown. */
#define PCL_ESHUTDOWN                         (PCL_ERR_NETSTART + 25)
/** Socket type not supported. */
#define PCL_ESOCKTNOSUPPORT                   (PCL_ERR_NETSTART + 26)
/** Stale remote resource, such as an NFS file. */
#define PCL_ESTALE                            (PCL_ERR_NETSTART + 27)
/** Too many references, can't splice. */
#define PCL_ETOOMANYREFS                      (PCL_ERR_NETSTART + 28)
/** Too many users. */
#define PCL_EUSERS                            (PCL_ERR_NETSTART + 29)
/** Authoritative Answer: Host not found. */
#define PCL_EHOSTNOTFOUND                     (PCL_ERR_NETSTART + 30)
/** Non-Authoritative: Host not found or SERVERFAIL. */
#define PCL_ETRYAGAIN                         (PCL_ERR_NETSTART + 31)
/** Non recoverable errors, FORMERR, REFUSED, NOTIMP. */
#define PCL_ENORECOVERY                       (PCL_ERR_NETSTART + 32)
/** The requested name is valid but does not have an IP address. */
#define PCL_ENODATA                           (PCL_ERR_NETSTART + 33)
/** No address, look for MX record. */
#define PCL_ENOADDRESS                        (PCL_ERR_NETSTART + 34)
/** Communication error on send */
#define PCL_ECOMM                             (PCL_ERR_NETSTART + 35)
/** Machine is not on the network */
#define PCL_ENONET                            (PCL_ERR_NETSTART + 36)
/** Name not unique on network */
#define PCL_ENOTUNIQ                          (PCL_ERR_NETSTART + 37)
/** Remote address changed */
#define PCL_EREMCHG                           (PCL_ERR_NETSTART + 38)
/** Remote I/O error */
#define PCL_EREMOTEIO                         (PCL_ERR_NETSTART + 39)

/** @} */
#endif // LIBPCL__ERRNO_H
