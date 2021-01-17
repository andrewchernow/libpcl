
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

#ifndef LIBPCL_NET_H
#define LIBPCL_NET_H

#include <pcl/types.h>
#ifdef PCL_UNIX
	#include <arpa/inet.h>
#else
	#include <ws2tcpip.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Converts an IP string address to a binary internet address.  af can
 * be AF_INET or AF_INET6.  This function is equivilant to inet_pton,
 * which is not always available.  Returns 0 on success and
 * -1 on error.  'inaddr' is expected to be sizeof(struct in_addr) if
 * AF_INET is specified and sizeof(struct in6_addr) if AF_INET6 is specified.
 * Supports AF_INET, AF_INET6.
 */
PCL_PUBLIC int pcl_net_ip2inet(int af, const char *ipstr, void *inaddr);

/* Converts a binary inet addr (in_addr or in6_addr) to a string.  Same as
 * inet_ntop, which is not always available. If af is AF_INET,
 * 'inaddr' is a struct in_addr, if AF_INET6 its a struct in6_addr.
 * Returns NULL on error.  NOTE: this wraps pcl_net_addr2ip().
 * Supports AF_INET, AF_INET6 and AF_UNIX.
 */
PCL_PUBLIC char *pcl_net_inet2ip(int af, const void *inaddr, char *ipbuf, size_t len);

/* Converts an IP or hostname to a binary socket address.  It is
 * recommended that if 'sa' is not NULL, is should be wide enough for an
 * IPv6 sockaddr_in6 or a AF_UNIX path. NOTE: sa can be NULL, which will just
 * verify the host. Returns zero on success and -1 on error.
 * Supports AF_INET, AF_INET6 and AF_UNIX.
 */
PCL_PUBLIC int pcl_net_host2addr(const char *host, struct sockaddr *sa);

/* Converts a sockaddr to its string representation and stores the
 * result at the address pointed to by ipbuf, which is len bytes
 * wide.  Returns a pointer to ipbuf on success and NULL on error.
 * Supports AF_INET, AF_INET6 and AF_UNIX.
 */
PCL_PUBLIC char *pcl_net_addr2ip(struct sockaddr *sa, char *ipbuf, size_t len);

/* Convert an AddrInfo (AI) error code to an PCL code */
PCL_PUBLIC int pcl_net_ai2pcl(int ai_retval);

/* Convert a DNS resolution error to an PCL error code */
PCL_PUBLIC int pcl_net_dns2pcl(int err);

/* performs reverse DNS on the give IPv4 or IPv6 address.  The resulting
 * hostname is written to host, no more than len bytes will be written.
 * return 0 on success and -1 on error, check pcl_errno.
 */
PCL_PUBLIC int pcl_net_reverse(const char *ipaddr, char *host, size_t len);

/** Resolve a hostname to an array of IPv4 and/or IPv6 addresses.
 * @param host pointer to a hostname
 * @return pointer to an array of IP addresses or \c NULL on error.
 */
PCL_PUBLIC pcl_array_t *pcl_net_resolve(const char *host);

/** Retrieve the DNS TXT records for a given host.
 * @param host pointer to a host
 * @return pointer to an array where each element is a TXT record or \c NULL on error.
 */
PCL_PUBLIC pcl_array_t *pcl_net_dnstxtrec(const char *host);

/* Gets the length in bytes of the given socket address.  Returns zero
 * if addr is NULL or the address family is not supported.  Supported
 * families are: AF_INET, AF_INET6 and AF_UNIX (non-windows systems only).
 */
PCL_PUBLIC socklen_t pcl_net_addrlen(const struct sockaddr *addr);

#ifdef __cplusplus
}
#endif

#endif
