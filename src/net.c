
/*
	Portable C Library (PCL)
	Copyright (c) 1999-2003, 2005-2014, 2017-2020 Andrew Chernow
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

#include <pcl/net.h>
#include <pcl/vector.h>
#include <pcl/error.h>
#include <pcl/string.h>
#include <pcl/stdlib.h>

#ifdef PCL_UNIX
	#ifdef PCL_DARWIN
		/* force arpa/nameser.h to include arpa/nameser_compat.h */
		#define BIND_8_COMPAT
	#endif
	#include <netdb.h>
	#include <arpa/nameser.h>
	#include <resolv.h>
	#include <pcl/limits.h>
	#include <sys/un.h>
#else
	#include <Windns.h>
#endif

int
pcl_net_ip2inet(int af, const char *ipstr, void *inaddr)
{
	int r;
	struct addrinfo *info = NULL, hints = {0};

	if(!inaddr)
		return BADARG();

	hints.ai_family = af;
	if((r = getaddrinfo(ipstr, NULL, &hints, &info)))
		return SETERR(pcl_net_ai2pcl(r));

	if(info->ai_family != af)
	{
		freeaddrinfo(info);
		return SETERR(PCL_ETYPE);
	}

	memcpy(inaddr, info->ai_addr, info->ai_addrlen);
	freeaddrinfo(info);
	return 0;
}

char *
pcl_net_inet2ip(int af, const void *inaddr, char *ipbuf, size_t len)
{
	char buf[sizeof(struct sockaddr_in6)];
	struct sockaddr *sa = (struct sockaddr *)buf;

	switch((sa->sa_family = (uint16_t)af))
	{
		case AF_INET:
			((struct sockaddr_in *)sa)->sin_addr = *(struct in_addr *)inaddr;
			break;

		case AF_INET6:
			((struct sockaddr_in6 *)sa)->sin6_addr = *(struct in6_addr *)inaddr;
			break;

		default:
			return R_SETERR(NULL, PCL_EAFNOSUPPORT);
	}

	if(!pcl_net_addr2ip(sa, ipbuf, len))
		return R_TRC(NULL);

	return ipbuf;
}

int
pcl_net_host2addr(const char *host, struct sockaddr *sa)
{
	if(strempty(host))
		return BADARG();

#ifdef PCL_UNIX
	if(*host == '/')
	{
		if(!sa)
		{
			if(strlen(host) >= PCL_MAXUNIXSOCKPATH)
				return SETERR(PCL_EOVERFLOW);
			return 0;
		}

		struct sockaddr_un *u = (struct sockaddr_un *)sa;
		u->sun_family = AF_UNIX;
		if(!pcl_strcpy(u->sun_path, sizeof(u->sun_path), host))
			return SETERR(PCL_EOVERFLOW);
		return 0;
	}
#endif

	struct addrinfo *info = NULL;
	int r = getaddrinfo(host, NULL, NULL, &info);
	if(r)
		return SETERR(pcl_net_ai2pcl(r));
	if(sa)
		memcpy(sa, info->ai_addr, info->ai_addrlen);
	freeaddrinfo(info);
	return 0;
}

char *
pcl_net_addr2ip(struct sockaddr *sa, char *ipbuf, size_t len)
{
	int err;
	char *p;

	if(!sa || !ipbuf || len < 1)
		return R_SETERR(NULL, PCL_EINVAL);

	*ipbuf = 0;

#ifdef PCL_UNIX
	if(sa->sa_family == AF_UNIX)
	{
		struct sockaddr_un *u = (struct sockaddr_un *)sa;
		if(!pcl_strcpy(ipbuf, len, u->sun_path))
			return R_SETERR(NULL, PCL_EBUF);
		return ipbuf;
	}
#endif

	if((err = getnameinfo(sa, pcl_net_addrlen(sa), ipbuf, (uint32_t)len, NULL, 0, NI_NUMERICHOST)))
		return R_SETERR(NULL, pcl_net_ai2pcl(err));

	if(sa->sa_family == AF_INET6 && (p = strrchr(ipbuf, '%')))
		*p = 0;

	return ipbuf;
}

int
pcl_net_reverse(const char *ipaddr, char *host, size_t len)
{
	int r;
	struct addrinfo *info = NULL;

	if(strempty(ipaddr))
		return BADARG();

	if((r = getaddrinfo(ipaddr, NULL, NULL, &info)))
		return SETERR(pcl_net_ai2pcl(r));

	r = getnameinfo(info->ai_addr, (socklen_t)info->ai_addrlen, host, (uint32_t)len, NULL, 0, NI_NAMEREQD);
	freeaddrinfo(info);

	if(r)
		return SETERR(pcl_net_ai2pcl(r));

	return 0;
}

pcl_vector_t *
ipcl_net_resolve(const char *host)
{
	int r;
	pcl_vector_t *addrs;
	struct addrinfo *info = NULL, *ai, hints = {0};

	hints.ai_socktype = SOCK_STREAM;
	if((r = getaddrinfo(host, NULL, &hints, &info)))
		return R_SETERR(NULL, pcl_net_ai2pcl(r));

	addrs = pcl_vector_create(3, sizeof(char **), pcl_vector_cleanup_dblptr);

	for(ai=info; ai; ai=ai->ai_next)
	{
		if(ai->ai_family == AF_INET || ai->ai_family == AF_INET6)
		{
			char ip[100];

			if(getnameinfo(ai->ai_addr, (socklen_t)ai->ai_addrlen, ip, (int)sizeof(ip), NULL, 0, NI_NUMERICHOST) == 0)
			{
				char *a = pcl_strdup(ip);
				pcl_vector_append(addrs, &a);
			}
		}
	}

	freeaddrinfo(info);

	return addrs;
}

pcl_vector_t *
pcl_net_dnstxtrec(const char *host)
{
	pcl_vector_t *vec = NULL;

#ifdef PCL_WINDOWS
	DNS_RECORD *rlist = NULL;
	DNS_STATUS status = DnsQuery_A(host, DNS_TYPE_TEXT, 0, NULL, &rlist, NULL);

	if(status)
		return R_SETOSERRMSG(NULL, status, "Cannot resolve: '%s'", host);

	DNS_TXT_DATA *data = &rlist->Data.Txt;
	vec = pcl_vector_create(data->dwStringCount, sizeof(char **), pcl_vector_cleanup_dblptr);

	for(DWORD i=0; i < data->dwStringCount; i++)
	{
		char *s = pcl_tcstoutf8(data->pStringArray[i], 0, NULL);
		pcl_vector_append(vec, &s);
	}

	DnsRecordListFree(rlist, DnsFreeRecordList);

#else
	ns_rr rr;
	ns_msg handle;
	uint8_t answer[65535];
	const uint8_t *data;
	int len = res_query(host, C_IN, T_TXT, answer, (int)sizeof(answer));

	if(len == -1)
		return R_SETERRMSG(NULL, pcl_net_dns2pcl(h_errno), "Cannot resolve: '%s'", host);

	if(ns_initparse(answer, len, &handle) < 0)
		return R_SETLASTERR(NULL);

	if(ns_parserr(&handle, ns_s_an, 0, &rr))
		return R_SETLASTERR(NULL);

	vec = pcl_vector_create(4, sizeof(char **), pcl_vector_cleanup_dblptr);
	len = ns_rr_rdlen(rr);
	data = ns_rr_rdata(rr);

	for(int n=0; n < len; )
	{
		int q = data[n++]; /* first byte is txt length */
		char *txt = pcl_strndup((const char *)(data + n), q);
		pcl_vector_append(vec, &txt);
		n += q;
	}

#endif

	return vec;
}

socklen_t
pcl_net_addrlen(const struct sockaddr *addr)
{
	if(!addr)
		return 0;

	if(addr->sa_family == AF_INET)
		return (socklen_t)sizeof(struct sockaddr_in);

	if(addr->sa_family == AF_INET6)
		return (socklen_t)sizeof(struct sockaddr_in6);

#ifdef PCL_UNIX
	if(addr->sa_family == AF_UNIX)
		return (socklen_t)sizeof(struct sockaddr_un);
#endif

	return 0;
}

int
pcl_net_dns2pcl(int err)
{
	switch(err)
	{
		case HOST_NOT_FOUND: return PCL_EHOSTNOTFOUND;
		case TRY_AGAIN:      return PCL_EAGAIN;
		case NO_RECOVERY:    return PCL_ENORECOVERY;
		case NO_DATA:        return PCL_ENODATA;
	}

	/* likely this is NETDB_INTERNAL on Unixes */
	return pcl_err_os2pcl(pcl_oserrno);
}

int
pcl_net_ai2pcl(int ai_retval)
{
	switch(ai_retval)
	{
#ifdef EAI_BADFLAGS
		case EAI_BADFLAGS:
			return PCL_EINVAL;
#endif

#ifdef EAI_NONAME
		case EAI_NONAME:
			return PCL_EHOSTNOTFOUND;
#endif

#ifdef EAI_AGAIN
		case EAI_AGAIN:
			return PCL_ETRYAGAIN;
#endif

#ifdef EAI_FAIL
		case EAI_FAIL:
			return PCL_EHOSTNOTFOUND;
#endif

#if defined(EAI_NODATA) && EAI_NODATA != EAI_NONAME
		case EAI_NODATA:
			return PCL_ENODATA;
#endif

#ifdef EAI_FAMILY
		case EAI_FAMILY:
			return PCL_EAFNOSUPPORT;
#endif

#ifdef EAI_SOCKTYPE
		case EAI_SOCKTYPE:
			return PCL_ESOCKTNOSUPPORT;
#endif

#ifdef EAI_SERVICE
		case EAI_SERVICE:
			return PCL_ESOCKTNOSUPPORT;
#endif

#ifdef EAI_ADDRFAMILY
		case EAI_ADDRFAMILY:
			return PCL_EAFNOSUPPORT;
#endif

#ifdef EAI_MEMORY
		case EAI_MEMORY:
			return PCL_ENOMEM;
#endif

#ifdef EAI_SYSTEM
		case EAI_SYSTEM:
			return pcl_err_os2pcl(errno);
#endif

#ifdef EAI_OVERFLOW
		case EAI_OVERFLOW:
			return PCL_EOVERFLOW;
#endif

#ifdef EAI_INPROGRESS
		case EAI_INPROGRESS:
			return PCL_EINPROGRESS;
#endif

#ifdef EAI_CANCELED
		case EAI_CANCELED:
			return PCL_ECANCELLED;
#endif

#ifdef EAI_NOTCANCELED
		case EAI_NOTCANCELED:
			return PCL_EINPROGRESS;
#endif

#ifdef EAI_ALLDONE
		case EAI_ALLDONE:
			return PCL_EOKAY;
#endif

#ifdef EAI_INTR
		case EAI_INTR:
			return PCL_EINTR;
#endif

#ifdef EAI_IDN_ENCODE
		case EAI_IDN_ENCODE:
			return PCL_EFORMAT;
#endif

		default:
			break;
	}

	return PCL_EUNDEF;
}
