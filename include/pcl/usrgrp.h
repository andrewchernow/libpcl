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

#ifndef LIBPCL_USRGRP_H
#define LIBPCL_USRGRP_H

#include <pcl/types.h>

#ifdef __cplusplus
extern "C" {
#endif

PCL_PUBLIC int pcl_getuser(const uid_t uid, char *out, size_t len);

PCL_PUBLIC int pcl_getgroup(const gid_t gid, char *out, size_t len);

/** Gets the user identifier for the given account user name.
 * @param name account user name
 * @param uidp pointer to a uid_t to receive the user identifier. This can be NULL, in which
 * case this function becomes an "exists" check.
 * @return 0 on success and -1 on failure. If the user referenced by name does not exist, the
 * pcl_errno is set to PCL_ENOTFOUND (not PCL_ENOENT, which the OS may set for other reasons).
 */
PCL_PUBLIC int pcl_getuid(const char *name, uid_t *uidp);

/** Gets the group identifier for the given group name.
 * @param name group name
 * @param gidp pointer to a gid_t to receive the group identifier. This can be NULL, in which
 * case this function becomes an "exists" check.
 * @return 0 on success and -1 on failure. If the group referenced by name does not exist, the
 * pcl_errno is set to PCL_ENOTFOUND (not PCL_ENOENT, which the OS may set for other reasons).
 */
PCL_PUBLIC int pcl_getgid(const char *name, gid_t *gidp);

#ifdef __cplusplus
}
#endif

#endif //__LIBPCL_USRGRP_H__
