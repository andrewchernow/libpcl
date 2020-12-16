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
#include <pcl/json.h>
#include <pcl/error.h>
#include <pcl/htable.h>
#include <pcl/array.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char **argv)
{
	pcl_init();

	UNUSED(argc || argv);

	FILE *fp = fopen("examples/x.json", "r");
	char buf[16 * 1024];

	int n = 0;

	//buf[n++] = '"';

	n += (int) fread(buf, 1, sizeof(buf), fp);

	//buf[n++] = '"';

	//fwrite(buf, 1, n, stdout);
	//printf("\n");

	const char *end;
	pcl_json_t *root = pcl_json_decode(buf, n, &end);

	if(!root)
		PANIC(NULL, 0);

	printf("%c: %1.15f\n", root->type, root->real);
	exit(0);
	printf("END = %s\n", end);

	pcl_json_t *jv = pcl_json_object_get(root, "stuff");

	printf("array-count=%d\n", jv->array->count);

	jv = pcl_json_array_get(jv, 0);

	pcl_array_t *keys = pcl_htable_keys(jv->object);

	for(int i = 0; i < keys->count; i++)
	{
		char *key = keys->elements[i];
		pcl_json_t *elem = pcl_htable_get(jv->object, key);
		if(elem->type == 's')
			printf("KEY = %s, VALUE = %s\n", key, elem->string);
	}

	pcl_array_free(keys);

	printf("type = %c\n", jv->type);
	printf("%s\n", ((pcl_json_t *) pcl_htable_get(jv->object, "name"))->string);

	char *out = pcl_json_encode(root, true);

	printf("%s\n", out);
	return 0;
}


