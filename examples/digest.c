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

#include <pcl/init.h>
#include <pcl/error.h>
#include <pcl/io.h>
#include <pcl/crypto.h>
#include <pcl/string.h>
#include <pcl/alloc.h>
#include <stdlib.h>

int pcl_tmain(int argc, pchar_t **argv)
{
	pchar_t *file = argc > 1 ? argv[1] : NULL;

	pcl_init();

	if(!file)
	{
		SETERR(PCL_EINVAL);
		pcl_err_fprintf(stderr, 0, "Missing required FILE argument");
		return 1;
	}

	FILE *fp = pcl_fopen(file, _P("rb"));

	if(!fp)
		PANIC("cannot open file '%Ps'", file);

	size_t r;
	char text[4096];
	pcl_digest_t *md[4];

	md[0] = pcl_digest_create("md5");
	md[1] = pcl_digest_create("sha1");
	md[2] = pcl_digest_create("sha256");
	md[3] = pcl_digest_create("sha512");

	while((r = fread(text, 1, sizeof(text), fp)) > 0)
	{
		for(int i = 0; i < countof(md); i++)
		{
			if(pcl_digest_update(md[i], text, (int) r) < 0)
				PANIC("%s update error", pcl_digest_name(md[i]));
		}
	}

	fclose(fp);

	for(int i = 0; i < countof(md); i++)
	{
		int len;
		char hex[200];
		void *digest = pcl_digest_final(md[i], &len);

		if(!digest)
			PANIC("%s final error", pcl_digest_name(md[i]));

		pcl_hex_encode(hex, sizeof(hex), digest, len);
		printf("%s: %s\n", pcl_digest_name(md[i]), hex);

		pcl_free(digest);
		pcl_digest_free(md[i]);
	}

	return 0;
}
