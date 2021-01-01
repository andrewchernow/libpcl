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
#include <pcl/array.h>
#include <pcl/vector.h>
#include <pcl/htable.h>
#include <string.h>

static void
walk_path(pcl_json_t *node, const pcl_json_path_t *path, pcl_array_t *results)
{
	switch(path->type)
	{
		case PclPathRoot:
		{
			if(path->next == NULL)
				pcl_array_add(results, pcl_json_ref(node, 1));
			else
				walk_path(node, path->next, results);

			break;
		}

		case PclPathMember:
		{
			if(pcl_json_isobj(node))
			{
				pcl_json_t *mbr = pcl_json_objget(node, path->member);

				if(mbr)
				{
					if(path->next == NULL)
						pcl_array_add(results, pcl_json_ref(mbr, 1));
					else
						walk_path(mbr, path->next, results);
				}
			}

			break;
		}

		case PclPathRecursiveDescent:
		{
			if(pcl_json_isobj(node))
			{
				for(int i = 0; i < node->object->capacity; i++)
				{
					pcl_htable_entry_t *ent = node->object->entries[i];

					for(; ent; ent = ent->next)
					{
						if(path->next->type == PclPathMember && !strcmp(path->next->member, ent->key))
							walk_path(node, path->next, results);
						else
							walk_path(ent->value, path, results);
					}
				}
			}
			else if(pcl_json_isarr(node))
			{
				for(int i = 0; i < node->array->count; i++)
				{
					pcl_json_t *elem = node->array->elements[i];

					if(path->next->type == PclPathElement && path->next->index == i)
						walk_path(node, path->next, results);
					else
						walk_path(elem, path, results);
				}
			}

			break;
		}

		case PclPathWildcardMember:
		{
			if(!pcl_json_isobj(node))
			{
				pcl_array_add(results, pcl_json_ref(node, 1));
				break;
			}

			for(int i = 0; i < node->object->capacity; i++)
			{
				pcl_htable_entry_t *ent = node->object->entries[i];

				for(; ent; ent = ent->next)
				{
					if(path->next)
						walk_path(ent->value, path->next, results);
					else
						pcl_array_add(results, pcl_json_ref(ent->value, 1));
				}
			}

			break;
		}

		case PclPathElement:
		{
			if(pcl_json_isarr(node))
			{
				pcl_err_freeze(true);
				pcl_json_t *elem = pcl_array_get(node->array, path->index);
				pcl_err_freeze(false);

				if(elem)
				{
					if(path->next == NULL)
						pcl_array_add(results, pcl_json_ref(elem, 1));
					else
						walk_path(elem, path->next, results);
				}
			}

			break;
		}

		case PclPathElementList:
		{
			if(!pcl_json_isarr(node))
				break;

			for(int i = 0; i < path->idx_list->count; i++)
			{
				int *index = pcl_vector_get(path->idx_list, i);

				pcl_err_freeze(true);
				pcl_json_t *elem = pcl_array_get(node->array, *index);
				pcl_err_freeze(false);

				if(elem)
				{
					if(path->next == NULL)
						pcl_array_add(results, pcl_json_ref(elem, 1));
					else
						walk_path(elem, path->next, results);
				}
			}

			break;
		}

		case PclPathWildcardElement:
		{
			if(!pcl_json_isarr(node))
			{
				pcl_array_add(results, pcl_json_ref(node, 1));
				break;
			}

			for(int i = 0; i < node->array->count; i++)
			{
				pcl_json_t *elem = node->array->elements[i];

				if(path->next == NULL)
					pcl_array_add(results, pcl_json_ref(elem, 1));
				else
					walk_path(elem, path->next, results);
			}

			break;
		}

		case PclPathElementSlice:
		{
			if(!pcl_json_isarr(node))
				break;

			int start = path->idx_slice.start;
			int end = path->idx_slice.end ? path->idx_slice.end : node->array->count;

			if(start < 0)
				start = node->array->count + path->idx_slice.start;

			if(end < 0)
				end = node->array->count + path->idx_slice.end;

			for(int i = start; i < end; i += path->idx_slice.step)
			{
				pcl_err_freeze(true);
				pcl_json_t *elem = pcl_array_get(node->array, i);
				pcl_err_freeze(false);

				if(elem)
				{
					if(path->next == NULL)
						pcl_array_add(results, pcl_json_ref(elem, 1));
					else
						walk_path(elem, path->next, results);
				}
			}

			break;
		}

		default:
			break;
	}
}

pcl_array_t *
pcl_json_match(pcl_json_t *root, const pcl_json_path_t *path)
{
	pcl_json_t *j = pcl_json_arr();
	pcl_array_t *results = j->array;

	j->array = NULL;
	pcl_json_free(j);

	walk_path(root, path, results);
	return results;
}
