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

#include "_ssl.h"
#include <pcl/vector.h>
#include <pcl/string.h>

pcl_vector_t *
pcl_ssl_certentry(pcl_ssl_t *ssl, int entry_type)
{
	X509 *cert;
	X509_NAME *name;
	int nid;
	int pos = -1;
	pcl_vector_t *ents = NULL;

	if(ssl == NULL)
	{
		BADARG();
		return NULL;
	}

	cert = SSL_get_peer_certificate(ssl->ssl);
	name = X509_get_subject_name(cert);

	switch(entry_type)
	{
		case PCL_SSL_CERT_COUNTRY_NAME:
			nid = NID_countryName;
			break;

		case PCL_SSL_CERT_LOCALITY_NAME:
			nid = NID_localityName;
			break;

		case PCL_SSL_CERT_STATE:
			nid = NID_stateOrProvinceName;
			break;

		case PCL_SSL_CERT_ORG_NAME:
			nid = NID_organizationName;
			break;

		case PCL_SSL_CERT_ORG_UNIT:
			nid = NID_organizationalUnitName;
			break;

		case PCL_SSL_CERT_COMMON_NAME:
			nid = NID_commonName;
			break;

		default:
			SETERRMSG(PCL_ETYPE, "Unknown certificate field: %d", entry_type);
			return NULL;
	}

	while(true)
	{
		ASN1_STRING *str;
		char *value;

		pos = X509_NAME_get_index_by_NID(name, nid, pos);
		if(pos < 0)
			break; /* end of list */

		if(!ents)
			ents = pcl_vector_create(X509_NAME_entry_count(name),
				sizeof(char**), pcl_vector_cleanup_dblptr);

		str = X509_NAME_ENTRY_get_data(X509_NAME_get_entry(name, pos));
		value = pcl_strndup((const char *)str->data, str->length);
		pcl_vector_append(ents, &value);
	}

	if(!ents)
		SETERR(PCL_ENOTFOUND);

	return ents;
}
