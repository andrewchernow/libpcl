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

#include "_json.h"
#include <pcl/string.h>
#include <pcl/vector.h>
#include <pcl/alloc.h>
#include <pcl/strint.h>

pcl_json_path_t *
pcl_json_compile(const char *path)
{
	const char *p = pcl_strskipws(path);

	if(p[0] != '$' || (p[1] != '.' && p[1] != '['))
		return R_SETERRMSG(NULL, PCL_ESYNTAX, "missing '$' root", 0);

	pcl_json_path_t *root = pcl_malloc(sizeof(pcl_json_path_t));

	root->next = NULL;
	root->type = PclPathRoot;

	for(p++; *p;)
	{
		switch(*p)
		{
			case '$':
				pcl_json_freepath(root);
				return R_SETERRMSG(NULL, PCL_ESYNTAX, "second '$' root not allowed", 0);

			case '.':
			{
				pcl_json_path_t *node = pcl_malloc(sizeof(pcl_json_path_t));

				if(p[1] == '.')
				{
					node->type = PclPathRecursiveDescent;
					p++; // don't skip 2nd dot, needed for next iteration
				}
				else if(p[1] == '*')
				{
					node->type = PclPathWildcardMember;
					p += 2;
				}
				else
				{
					const char *end = ++p;

					while(*end != '.' && *end != '[' && *end)
						end++;

					if(end == p)
					{
						pcl_free(node);
						pcl_json_freepath(root);
						return R_SETERRMSG(NULL, PCL_ESYNTAX, "missing member name", 0);
					}

					node->type = PclPathMember;
					node->member = pcl_strndup(p, end - p);
					p = end;
				}

				node->next = root;
				root = node;
				break;
			}

			case '[':
			{
				char *end;
				pcl_json_path_t *node = pcl_malloc(sizeof(pcl_json_path_t));

				/* $.book[*] */
				if(p[1] == '*' && p[2] == ']')
				{
					node->type = PclPathWildcardElement;
					p += 3;
				}
				else if(p[1] == '\'')
				{
					/* $.book['*'] */
					if(p[2] == '*' && p[3] == '\'' && p[4] == ']')
					{
						node->type = PclPathWildcardMember;
						p += 5;
					}
					else
					{
						p += 2;

						end = strchr(p, '\'');

						if(!end)
						{
							pcl_free(node);
							pcl_json_freepath(root);
							return R_SETERRMSG(NULL, PCL_ESYNTAX, "unterminated single quote", 0);
						}

						node->type = PclPathMember;
						node->member = pcl_strndup(p, end - p);
						p = end + 1;
					}
				}
					/* slice with no start $.book[:2] */
				else if(p[1] == ':')
				{
					int slice_end = 0;
					int slice_step = 1;

					p += 2; // skip "[:"
					end = (char *) p;

					/* if end is ':', then we have [::N] */
					if(*end != ':' && pcl_strtoi(end, &end, 10, &slice_end) < 0)
					{
						pcl_free(node);
						pcl_json_freepath(root);
						return R_TRCMSG(NULL, "invalid slice expression: slice end", 0);
					}

					p = end;

					/* slice step - $.book[:1:2] */
					if(*p == ':')
					{
						if(pcl_strtoi(p + 1, &end, 10, &slice_step) < 0)
						{
							pcl_free(node);
							pcl_json_freepath(root);
							return R_TRCMSG(NULL, "invalid slice expression: slice step", 0);
						}

						p = end;
					}

					if(*p++ != ']')
					{
						pcl_free(node);
						pcl_json_freepath(root);
						return R_SETERRMSG(NULL, PCL_ESYNTAX, "invalid slice expression: no closing ']'", 0);
					}

					node->type = PclPathElementSlice;
					node->idx_slice.start = 0;
					node->idx_slice.end = slice_end;
					node->idx_slice.step = slice_step;
				}
				else
				{
					int index;

					if(pcl_strtoi(p + 1, &end, 10, &index) < 0)
					{
						pcl_free(node);
						pcl_json_freepath(root);
						return R_TRCMSG(NULL, "invalid subscript expression", 0);
					}

					p = end;

					if(*p == ',')
					{
						pcl_vector_t *indexes = pcl_vector_create(4, sizeof(int), NULL);

						pcl_vector_append(indexes, &index);

						for(; *p != ']'; p = end)
						{
							if(pcl_strtoi(p + 1, &end, 10, &index) < 0)
							{
								pcl_free(node);
								pcl_json_freepath(root);
								pcl_vector_free(indexes);
								return R_TRCMSG(NULL, "invalid subscript set expression", 0);
							}

							if(*end != ',' && *end != ']')
							{
								pcl_free(node);
								pcl_json_freepath(root);
								pcl_vector_free(indexes);
								return R_SETERRMSG(NULL, PCL_ESYNTAX, "invalid subscript set expression", 0);
							}

							pcl_vector_append(indexes, &index);
						}

						node->type = PclPathElementList;
						node->idx_list = indexes;
						p++;
					}
					else if(*p == ':')
					{
						int slice_end = 0;
						int slice_step = 1;

						end = (char *) p + 1;

						if(*end != ']' && *end != ':' && pcl_strtoi(end, &end, 10, &slice_end) < 0)
						{
							pcl_free(node);
							pcl_json_freepath(root);
							return R_TRCMSG(NULL, "invalid slice expression: slice end", 0);
						}

						p = end;

						/* slice step - $.book[1:1:2] */
						if(*p == ':')
						{
							if(pcl_strtoi(p + 1, &end, 10, &slice_step) < 0)
							{
								pcl_free(node);
								pcl_json_freepath(root);
								return R_TRCMSG(NULL, "invalid slice expression: slice step", 0);
							}

							p = end;
						}

						if(*p != ']')
						{
							pcl_free(node);
							pcl_json_freepath(root);
							return R_SETERRMSG(NULL, PCL_ESYNTAX, "invalid slice expression: no closing ']'", 0);
						}

						p++;
						node->type = PclPathElementSlice;
						node->idx_slice.start = index;
						node->idx_slice.end = slice_end;
						node->idx_slice.step = slice_step;
					}
					else if(*p == ']')
					{
						p++;
						node->type = PclPathElement;
						node->index = index;
					}
					else
					{
						pcl_free(node);
						pcl_json_freepath(root);
						return R_SETERRMSG(NULL, PCL_ESYNTAX, "invalid subscript expression", 0);
					}
				}

				node->next = root;
				root = node;
				break;
			}

			default:
				pcl_json_freepath(root);
				return R_SETERRMSG(NULL, PCL_ESYNTAX, "unexpected character '%c'", *p);
		}
	}

	/* reverse order */
	for(pcl_json_path_t *prev = NULL, *current = root; current;)
	{
		pcl_json_path_t *next = current->next;
		current->next = prev;
		root = prev = current;
		current = next;
	}

	return root;
}
























