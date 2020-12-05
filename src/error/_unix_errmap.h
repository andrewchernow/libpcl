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


#ifdef E2BIG
_errmap(E2BIG,            PCL_E2BIG)
#endif

#ifdef EACCES
_errmap(EACCES,           PCL_EACCES)
#endif

#ifdef EAGAIN
_errmap(EAGAIN,           PCL_EAGAIN)
#endif

#ifdef EALREADY
_errmap(EALREADY,         PCL_EALREADY)
#endif

#ifdef EAUTH
_errmap(EAUTH,            PCL_EAUTH)
#endif

#ifdef EBADARCH
_errmap(EBADARCH,         PCL_ENOEXEC)
#endif

#ifdef EBADEXEC
_errmap(EBADEXEC,         PCL_ENOEXEC)
#endif

#ifdef EBADF
_errmap(EBADF,            PCL_EBADF)
#endif

#ifdef EBADFD
_errmap(EBADFD,           PCL_EBADF)
#endif

#ifdef EBADMACHO
_errmap(EBADMACHO,        PCL_EFORMAT)
#endif

#ifdef EBADMSG
_errmap(EBADMSG,          PCL_EINVAL)
#endif

#ifdef EBADR
_errmap(EBADR,            PCL_EBADCMD)
#endif

#ifdef EBADRPC
_errmap(EBADRPC,          PCL_EFORMAT)
#endif

#ifdef EBADRQC
_errmap(EBADRQC,          PCL_EBADCMD)
#endif

#ifdef EBADSLT
_errmap(EBADSLT,          PCL_EBADCMD)
#endif

#ifdef EBFONT
_errmap(EBFONT,           PCL_ENOEXEC)
#endif

#ifdef EBUSY
_errmap(EBUSY,            PCL_EBUSY)
#endif

#ifdef ECANCELED
_errmap(ECANCELED,        PCL_ECANCELLED)
#endif

#ifdef ECHILD
_errmap(ECHILD,           PCL_ECHILD)
#endif

#ifdef ECHRNG
_errmap(ECHRNG,           PCL_ERANGE)
#endif

#ifdef EDEADLK
_errmap(EDEADLK,          PCL_EDEADLK)
#endif

#ifdef EDEVERR
_errmap(EDEVERR,          PCL_EDEVERR)
#endif

#ifdef EDIED
_errmap(EDIED,            PCL_EIO)
#endif

#ifdef EDOM
_errmap(EDOM,             PCL_EINVAL)
#endif

#ifdef EDQUOT
_errmap(EDQUOT,           PCL_EDQUOT)
#endif

#ifdef EEXIST
_errmap(EEXIST,           PCL_EEXIST)
#endif

#ifdef EFAULT
_errmap(EFAULT,           PCL_EFAULT)
#endif

#ifdef EFBIG
_errmap(EFBIG,            PCL_EFBIG)
#endif

#ifdef EFTYPE
_errmap(EFTYPE,           PCL_EFORMAT)
#endif

#ifdef EIDRM
_errmap(EIDRM,            PCL_ENOTFOUND)
#endif

#ifdef EILSEQ
_errmap(EILSEQ,           PCL_EILSEQ)
#endif

#ifdef EINPROGRESS
_errmap(EINPROGRESS,      PCL_EINPROGRESS)
#endif

#ifdef EINTR
_errmap(EINTR,            PCL_EINTR)
#endif

#ifdef EINVAL
_errmap(EINVAL,           PCL_EINVAL)
#endif

#ifdef EIO
_errmap(EIO,              PCL_EIO)
#endif

#ifdef EISDIR
_errmap(EISDIR,           PCL_EISDIR)
#endif

#ifdef EKEYEXPIRED
_errmap(EKEYEXPIRED,      PCL_EEXPIRED)
#endif

#ifdef EKEYREJECTED
_errmap(EKEYREJECTED,     PCL_EREJECTED)
#endif

#ifdef EKEYREVOKED
_errmap(EKEYREVOKED,      PCL_EREVOKED)
#endif

#ifdef EL2NSYNC
_errmap(EL2NSYNC,         PCL_ESYNC)
#endif

#ifdef ELIBACC
_errmap(ELIBACC,          PCL_EACCES)
#endif

#ifdef ELIBBAD
_errmap(ELIBBAD,          PCL_ENOEXEC)
#endif

#ifdef ELIBEXEC
_errmap(ELIBEXEC,         PCL_ENOEXEC)
#endif

#ifdef ELIBSCN
_errmap(ELIBSCN,          PCL_ENOEXEC)
#endif

#ifdef ELIBMAX
_errmap(ELIBMAX,          PCL_ELIBMAX)
#endif

#ifdef ELNRNG
_errmap(ELNRNG,           PCL_ERANGE)
#endif

#ifdef ELOOP
_errmap(ELOOP,            PCL_ELOOP)
#endif

#ifdef EMEDIUMTYPE
_errmap(EMEDIUMTYPE,      PCL_ETYPE)
#endif

#ifdef EMFILE
_errmap(EMFILE,           PCL_EMFILE)
#endif

#ifdef EMLINK
_errmap(EMLINK,           PCL_EMLINK)
#endif

#ifdef ENAMETOOLONG
_errmap(ENAMETOOLONG,     PCL_ENAMETOOLONG)
#endif

#ifdef ENAVAIL
_errmap(ENAVAIL,          PCL_EUNAVAIL)
#endif

#ifdef ENEEDAUTH
_errmap(ENEEDAUTH,        PCL_EAUTH)
#endif

#ifdef ENFILE
_errmap(ENFILE,           PCL_ENFILE)
#endif

#ifdef ENOANO
_errmap(ENOANO,          PCL_ENOTFOUND)
#endif

#ifdef ENOATTR
_errmap(ENOATTR,          PCL_ENOTFOUND)
#endif

#ifdef ENOCSI
_errmap(ENOCSI,          PCL_EUNAVAIL)
#endif

#ifdef ENODATA
_errmap(ENODATA,          PCL_ENODATA)
#endif

#ifdef ENODEV
_errmap(ENODEV,           PCL_ENODEV)
#endif

#ifdef ENOENT
_errmap(ENOENT,           PCL_ENOENT)
#endif

#ifdef ENOEXEC
_errmap(ENOEXEC,          PCL_ENOEXEC)
#endif

#ifdef ENOKEY
_errmap(ENOKEY,           PCL_EUNAVAIL)
#endif

#ifdef ENOLCK
_errmap(ENOLCK,           PCL_ENOLCK)
#endif

#ifdef ENOLINK
_errmap(ENOLINK,           PCL_ENOLINK)
#endif

#ifdef ENOMEDIUM
_errmap(ENOMEDIUM,        PCL_ENOTFOUND)
#endif

#ifdef ENOMEM
_errmap(ENOMEM,           PCL_ENOMEM)
#endif

#ifdef ENOMSG
_errmap(ENOMSG,           PCL_ETYPE)
#endif

#ifdef ENOPKG
_errmap(ENOPKG,           PCL_EUNAVAIL)
#endif

#ifdef ENOSPC
_errmap(ENOSPC,           PCL_ENOSPC)
#endif

#ifdef ENOSR
_errmap(ENOSR,            PCL_ENOSR)
#endif

#ifdef ENOSTR
_errmap(ENOSTR,           PCL_ENOSTR)
#endif

#ifdef ENOSYS
_errmap(ENOSYS,           PCL_ENOIMPL)
#endif

#ifdef ENOTBLK
_errmap(ENOTBLK,          PCL_ETYPE)
#endif

#ifdef ENOTDIR
_errmap(ENOTDIR,          PCL_ENOTDIR)
#endif

#ifdef ENOTEMPTY
_errmap(ENOTEMPTY,        PCL_ENOTEMPTY)
#endif

#ifdef ENOTNAM
_errmap(ENOTNAM,          PCL_ETYPE)
#endif

#ifdef ENOTSUP
_errmap(ENOTSUP,          PCL_ENOTSUP)
#endif

#ifdef ENOTTY
_errmap(ENOTTY,           PCL_EIO)
#endif

#ifdef ENXIO
_errmap(ENXIO,            PCL_ENODEV)
#endif

#ifdef EOVERFLOW
_errmap(EOVERFLOW,        PCL_EOVERFLOW)
#endif

#ifdef EPERM
_errmap(EPERM,            PCL_EPERM)
#endif

#ifdef EPIPE
_errmap(EPIPE,            PCL_EPIPE)
#endif

#ifdef EPROCLIM
_errmap(EPROCLIM,         PCL_EPROCLIM)
#endif

#ifdef EPROCUNAVAIL
_errmap(EPROCUNAVAIL,     PCL_EUNAVAIL)
#endif

#ifdef EPROGMISMATCH
_errmap(EPROGMISMATCH,    PCL_EVERSION)
#endif

#ifdef EPROGUNAVAIL
_errmap(EPROGUNAVAIL,     PCL_EUNAVAIL)
#endif

#ifdef EPWROFF
_errmap(EPWROFF,          PCL_EDEVERR)
#endif

#ifdef ERANGE
_errmap(ERANGE,           PCL_ERANGE)
#endif

#ifdef ERESTART
_errmap(ERESTART,         PCL_ERESTART)
#endif

#ifdef ERPCMISMATCH
_errmap(ERPCMISMATCH,     PCL_EVERSION)
#endif

#ifdef EROFS
_errmap(EROFS,            PCL_EROFS)
#endif

#ifdef ESHLIBVERS
_errmap(ESHLIBVERS,       PCL_EVERSION)
#endif

#ifdef ESPIPE
_errmap(ESPIPE,           PCL_ESPIPE)
#endif

#ifdef ESRCH
_errmap(ESRCH,            PCL_ESRCH)
#endif

#ifdef ESTRPIPE
_errmap(ESTRPIPE,         PCL_EPIPE)
#endif

#ifdef ETXTBSY
_errmap(ETXTBSY,          PCL_EBUSY)
#endif

#ifdef EUNATCH
_errmap(EUNATCH,          PCL_EUNAVAIL)
#endif

#if defined(EWOULDBLOCK) && defined(EAGAIN) && EWOULDBLOCK != EAGAIN
_errmap(EWOULDBLOCK,      PCL_EWOULDBLOCK)
#endif

#ifdef EXDEV
_errmap(EXDEV,            PCL_EXDEV)
#endif


/* --------------------------------------
 * Network Errors
 */

#ifdef EADDRINUSE
_errmap(EADDRINUSE,       PCL_EADDRINUSE)
#endif

#ifdef EADDRNOTAVAIL
_errmap(EADDRNOTAVAIL,    PCL_EADDRNOTAVAIL)
#endif

#ifdef EAFNOSUPPORT
_errmap(EAFNOSUPPORT,     PCL_EAFNOSUPPORT)
#endif

#ifdef ECOMM
_errmap(ECOMM,            PCL_ECOMM)
#endif

#ifdef ECONNABORTED
_errmap(ECONNABORTED,     PCL_ECONNABORTED)
#endif

#ifdef ECONNREFUSED
_errmap(ECONNREFUSED,     PCL_ECONNREFUSED)
#endif

#ifdef ECONNRESET
_errmap(ECONNRESET,       PCL_ECONNRESET)
#endif

#ifdef EDESTADDRREQ
_errmap(EDESTADDRREQ,     PCL_EDESTADDRREQ)
#endif

#ifdef EHOSTDOWN
_errmap(EHOSTDOWN,        PCL_EHOSTDOWN)
#endif

#ifdef EHOSTUNREACH
_errmap(EHOSTUNREACH,     PCL_EHOSTUNREACH)
#endif

#ifdef EISCONN
_errmap(EISCONN,          PCL_EISCONN)
#endif

#ifdef EMSGSIZE
_errmap(EMSGSIZE,         PCL_EMSGSIZE)
#endif

#ifdef EMULTIHOP
_errmap(EMULTIHOP,        PCL_EMULTIHOP)
#endif

#ifdef ENETDOWN
_errmap(ENETDOWN,         PCL_ENETDOWN)
#endif

#ifdef ENETRESET
_errmap(ENETRESET,        PCL_ENETRESET)
#endif

#ifdef ENETUNREACH
_errmap(ENETUNREACH,      PCL_ENETUNREACH)
#endif

#ifdef ENOBUFS
_errmap(ENOBUFS,          PCL_ENOBUFS)
#endif

#ifdef ENONET
_errmap(ENONET,           PCL_ENONET)
#endif

#ifdef ENOPROTOOPT
_errmap(ENOPROTOOPT,      PCL_ENOPROTOOPT)
#endif

#ifdef ENOTCONN
_errmap(ENOTCONN,         PCL_ENOTCONN)
#endif

#ifdef ENOTSOCK
_errmap(ENOTSOCK,         PCL_ENOTSOCK)
#endif

#ifdef ENOTUNIQ
_errmap(ENOTUNIQ,         PCL_ENOTUNIQ)
#endif

#ifdef EOPNOTSUPP
_errmap(EOPNOTSUPP,       PCL_EOPNOTSUPP)
#endif

#ifdef EPFNOSUPPORT
_errmap(EPFNOSUPPORT,     PCL_EPFNOSUPPORT)
#endif

#ifdef EPROTO
_errmap(EPROTO,           PCL_EPROTO)
#endif

#ifdef EPROTONOSUPPORT
_errmap(EPROTONOSUPPORT,  PCL_EPROTONOSUPPORT)
#endif

#ifdef EPROTOTYPE
_errmap(EPROTOTYPE,       PCL_EPROTOTYPE)
#endif

#ifdef EREMCHG
_errmap(EREMCHG,          PCL_EREMCHG)
#endif

#ifdef EREMOTE
_errmap(EREMOTE,          PCL_EREMOTE)
#endif

#ifdef EREMOTEIO
_errmap(EREMOTEIO,        PCL_EREMOTEIO)
#endif

#ifdef ESHUTDOWN
_errmap(ESHUTDOWN,        PCL_ESHUTDOWN)
#endif

#ifdef ESOCKTNOSUPPORT
_errmap(ESOCKTNOSUPPORT,  PCL_ESOCKTNOSUPPORT)
#endif

#ifdef ESTALE
_errmap(ESTALE,           PCL_ESTALE)
#endif

#ifdef ETIME
_errmap(ETIME,            PCL_ETIMEOUT)
#endif

#ifdef ETIMEDOUT
_errmap(ETIMEDOUT,        PCL_ETIMEOUT)
#endif

#ifdef ETOOMANYREFS
_errmap(ETOOMANYREFS,     PCL_ETOOMANYREFS)
#endif

#ifdef EUSERS
_errmap(EUSERS,           PCL_EUSERS)
#endif

