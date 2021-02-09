/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2021 Andrew Chernow
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


#define mkmsg(_err, _msg) {_err, #_err, _msg}

/* PCL ERRORS */
mkmsg(PCL_EOKAY,              "The operation was successful."),
mkmsg(PCL_E2BIG,              "Arg list too long."),
mkmsg(PCL_EACCES,             "Access is denied."),
mkmsg(PCL_EAGAIN,             "Try again."),
mkmsg(PCL_EALREADY,           "Operation is already in progress."),
mkmsg(PCL_EBADF,              "Bad file descriptor."),
mkmsg(PCL_EBADCMD,            "Invalid request."),
mkmsg(PCL_EBUSY,              "Device or resource busy."),
mkmsg(PCL_ECANCELLED,         "Operation cancelled."),
mkmsg(PCL_ECHILD,             "No child processes."),
mkmsg(PCL_EDEADLK,            "Resource deadlock would occur."),
mkmsg(PCL_EDEVERR,            "Device error."),
mkmsg(PCL_EDQUOT,             "Disc quota exceeded."),
mkmsg(PCL_EEXIST,             "Object or resource already exists."),
mkmsg(PCL_EFAULT,             "Bad Address."),
mkmsg(PCL_EFBIG,              "File too large."),
mkmsg(PCL_EILSEQ,             "Invalid or incomplete multibyte or wide character."),
mkmsg(PCL_ERESTART,           "Interrupted system call should be restarted"),
mkmsg(PCL_EINPROGRESS,        "Operation is now in progress."),
mkmsg(PCL_EINTR,              "Interrupted system call."),
mkmsg(PCL_EINVAL,             "Invalid argument."),
mkmsg(PCL_EIO,                "I/O error."),
mkmsg(PCL_EISDIR,             "Is a directory."),
mkmsg(PCL_ELIBMAX,            "Attempting to link in too many shared libraries."),
mkmsg(PCL_ELOOP,              "Too many symbolic links."),
mkmsg(PCL_EMFILE,             "Process open file limit has been exceeded."),
mkmsg(PCL_EMLINK,             "Too many hard links to the given file or directory."),
mkmsg(PCL_ENFILE,             "System open file limit has been exceeded."),
mkmsg(PCL_ENODEV,             "No such device."),
mkmsg(PCL_ENOENT,             "No such file or directory."),
mkmsg(PCL_ENOEXEC,            "Exec format error."),
mkmsg(PCL_ENOLCK,             "No locks available."),
mkmsg(PCL_ENOLINK,            "Link has been severed."),
mkmsg(PCL_ENOMEM,             "Out of memory."),
mkmsg(PCL_ESRCH,              "No such process."),
mkmsg(PCL_ENOSPC,             "No space left on device."),
mkmsg(PCL_ENOSR,              "Out of streams resources"),
mkmsg(PCL_ENOSTR,             "Not a stream."),
mkmsg(PCL_ENOIMPL,            "Operation/Function not implemented."),
mkmsg(PCL_ENOTDIR,            "Not a directory."),
mkmsg(PCL_ENOTEMPTY,          "Directory is not empty."),
mkmsg(PCL_ENOTSUP,            "Operation not supported."),
mkmsg(PCL_EOVERFLOW,          "Value too large to be stored in data type."),
mkmsg(PCL_EPERM,              "Permission denied."),
mkmsg(PCL_EPIPE,              "Broken pipe."),
mkmsg(PCL_EPROCLIM,           "Too many processes."),
mkmsg(PCL_ERANGE,             "Math result not representable."),
mkmsg(PCL_EROFS,              "Read-only file system."),
mkmsg(PCL_ESPIPE,             "Illegal seek."),
mkmsg(PCL_ETIMEOUT,           "Operation timed out."),
mkmsg(PCL_EXDEV,              "Cross-device operation not permitted."),
mkmsg(PCL_ENAMETOOLONG,       "Name is too long."),
mkmsg(PCL_EINDEX,             "Invalid index."),
mkmsg(PCL_EBUF,               "Buffer too small."),
mkmsg(PCL_EFORMAT,            "Invalid or corrupt format."),
mkmsg(PCL_ETYPE,              "Invalid type."),
mkmsg(PCL_ENOMORE,            "No more items."),
mkmsg(PCL_ENOTREADY,          "Device or resource is not ready or initialized."),
mkmsg(PCL_EPATH,              "Invalid pathname."),
mkmsg(PCL_EUNDEF,             "Undefined error occurred."),
mkmsg(PCL_ENOTFOUND,          "Object or resource not found."),
mkmsg(PCL_EOF,                "End of file."),
mkmsg(PCL_ECRYPT,             "Encryption or decryption failed."),
mkmsg(PCL_EAUTH,              "Authentication error."),
mkmsg(PCL_EVERSION,           "Version conflict."),
mkmsg(PCL_EUNAVAIL,           "Object or resource is unavailable."),
mkmsg(PCL_EEXPIRED,           "Object or resource has expired."),
mkmsg(PCL_EREJECTED,          "Object or resource has been rejected."),
mkmsg(PCL_EREVOKED,           "Object or resource has been revoked."),
mkmsg(PCL_ESYNC,              "Out of sync."),
mkmsg(PCL_EDISABLED,          "Object or resource has been disabled."),
mkmsg(PCL_EDB,                "Database error."),
mkmsg(PCL_ELOCK,              "The resource is currently locked."),
mkmsg(PCL_EABORT,             "The operation has been aborted."),
mkmsg(PCL_EDIGEST,            "Message digest failure."),
mkmsg(PCL_ESECURITY,          "Security violation."),
mkmsg(PCL_EMOVED,             "Object or resource has moved."),
mkmsg(PCL_ECLASS,             "Object is not an instance of class."),
mkmsg(PCL_ESSL,               "SSL error."),
mkmsg(PCL_ESEQ,               "Out of sequence."),
mkmsg(PCL_ESYNTAX,            "Syntax error."),
mkmsg(PCL_EBADH,              "Bad handle value."),
mkmsg(PCL_EDBQUERY,           "Database query error."),
mkmsg(PCL_EINVCODEPT,          "Invalid unicode codepoint."),

/* NETWORK ERRORS */
mkmsg(PCL_EADDRINUSE,         "The internet address is already in use."),
mkmsg(PCL_EADDRNOTAVAIL,      "Can't assign the requested internet address."),
mkmsg(PCL_EAFNOSUPPORT,       "Internet address family not supported by protocol family."),
mkmsg(PCL_ECOMM,              "Communication error on send."),
mkmsg(PCL_ECONNABORTED,       "Software caused connection abort."),
mkmsg(PCL_ECONNREFUSED,       "Connection refused."),
mkmsg(PCL_ECONNRESET,         "Connection reset by peer."),
mkmsg(PCL_EDESTADDRREQ,       "Destination internet address required."),
mkmsg(PCL_EHOSTDOWN,          "Host is down."),
mkmsg(PCL_EHOSTUNREACH,       "No route to host."),
mkmsg(PCL_EISCONN,            "Socket is already connected."),
mkmsg(PCL_EMSGSIZE,           "Message too long."),
mkmsg(PCL_EMULTIHOP,          "Hopping to multiple remote machines is prohibited."),
mkmsg(PCL_ENETDOWN,           "Network is down."),
mkmsg(PCL_ENETRESET,          "Network dropped connection on reset."),
mkmsg(PCL_ENETUNREACH,        "Network is unreachable."),
mkmsg(PCL_ENOBUFS,            "No buffer space available."),
mkmsg(PCL_ENONET,             "Machine is not on the network."),
mkmsg(PCL_ENOPROTOOPT,        "Protocol not available."),
mkmsg(PCL_ENOTCONN,           "Socket is not connected."),
mkmsg(PCL_ENOTSOCK,           "A socket operation was attempted on a non-socket."),
mkmsg(PCL_ENOTUNIQ,           "Name not unique on network."),
mkmsg(PCL_EOPNOTSUPP,         "Operation not supported."),
mkmsg(PCL_EPFNOSUPPORT,       "Protocol family not supported."),
mkmsg(PCL_EPROTO,             "Protocol not supported."),
mkmsg(PCL_EPROTONOSUPPORT,    "Protocol error."),
mkmsg(PCL_EPROTOTYPE,         "Protocol wrong type for socket."),
mkmsg(PCL_EREMCHG,            "Remote address changed."),
mkmsg(PCL_EREMOTE,            "Too many levels of remote in path."),
mkmsg(PCL_EREMOTEIO,          "Remote I/O error."),
mkmsg(PCL_ESHUTDOWN,          "Can't send after socket shutdown."),
mkmsg(PCL_ESOCKTNOSUPPORT,    "Socket type not supported."),
mkmsg(PCL_ESTALE,             "Stale remote resource, such as an NFS file."),
mkmsg(PCL_ETOOMANYREFS,       "Too many references, can't splice."),
mkmsg(PCL_EUSERS,             "Too many users."),
mkmsg(PCL_EHOSTNOTFOUND,      "Authoritative Answer: Host not found."),
mkmsg(PCL_ETRYAGAIN,          "Non-Authoritative: Host not found or SERVERFAIL."),
mkmsg(PCL_ENORECOVERY,        "Non recoverable errors, FORMERR, REFUSED, NOTIMP."),
mkmsg(PCL_ENODATA,            "The requested name is valid but does not have an IP address."),
mkmsg(PCL_ENOADDRESS,         "No address, look for MX record."),

/* place holder for gnu compiler */
{0, NULL, NULL}

