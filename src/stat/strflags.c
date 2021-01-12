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

#include <pcl/stat.h>
#include <pcl/string.h>

char *
pcl_strflags(char *out, size_t len, uint32_t flags)
{
	if(!out || len == 0)
		return out;

#define catflag(f) if((flags & (f)) && !pcl_strcat(out, len, *out ? "," #f : #f)) return out

	*out = 0;
	catflag(UF_NODUMP);
	catflag(UF_IMMUTABLE);
	catflag(UF_APPEND);
	catflag(UF_OPAQUE);
	catflag(UF_NOUNLINK);
	catflag(UF_COMPRESSED);
	catflag(UF_TRACKED);
	catflag(UF_ENCRYPTED);
	catflag(UF_SPARSE);
	catflag(UF_SYSTEM);
	catflag(UF_HIDDEN);
	catflag(SF_ARCHIVED);
	catflag(SF_IMMUTABLE);
	catflag(SF_APPEND);
	catflag(SF_NOUNLINK);
	catflag(SF_SNAPSHOT);
	return out;

#undef catflag
}
