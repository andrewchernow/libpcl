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

#include "_net.h"
#include <pcl/array.h>
#include <pcl/string.h>
#include <pcl/alloc.h>

#ifdef PCL_UNIX
#	ifdef PCL_DARWIN
		/* force arpa/nameser.h to include arpa/nameser_compat.h */
#		define BIND_8_COMPAT
#	endif
#	include <netdb.h>
#	include <arpa/nameser.h>
#	include <resolv.h>
#else
#	include <Windns.h>
#endif

pcl_array_t *
pcl_net_dnstxtrec(const char *host)
{
	pcl_array_t *arr;

#ifdef PCL_WINDOWS
	DNS_RECORD *rlist = NULL;
	DNS_STATUS status = DnsQuery_A(host, DNS_TYPE_TEXT, 0, NULL, &rlist, NULL);

	if(status)
		return R_SETOSERRMSG(NULL, status, "Cannot resolve: '%s'", host);

	DNS_TXT_DATA *data = &rlist->Data.Txt;
	arr = pcl_array(data->dwStringCount, pcl_cleanup_ptr);

	for(DWORD i=0; i < data->dwStringCount; i++)
		pcl_array_append(arr, pcl_pcs_to_utf8(data->pStringArray[i], 0, NULL));

	DnsRecordListFree(rlist, DnsFreeRecordList);

#else
	ns_rr rr;
	ns_msg handle;
	uint8_t answer[65535];
	const uint8_t *data;
	int len = res_query(host, C_IN, T_TXT, answer, (int) sizeof(answer));

	if(len == -1)
		return R_SETERRMSG(NULL, pcl_net_dns2pcl(h_errno), "Cannot resolve: '%s'", host);

	if(ns_initparse(answer, len, &handle) < 0)
		return R_SETLASTERR(NULL);

	if(ns_parserr(&handle, ns_s_an, 0, &rr))
		return R_SETLASTERR(NULL);

	arr = pcl_array(4, pcl_cleanup_ptr);
	len = ns_rr_rdlen(rr);
	data = ns_rr_rdata(rr);

	for(int n = 0; n < len;)
	{
		int q = data[n++]; /* first byte is txt length */
		pcl_array_append(arr, pcl_strndup((const char *) (data + n), q));
		n += q;
	}
#endif

	return arr;
}
