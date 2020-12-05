/*
  Portable C Library ("PCL")
  Copyright (c) 1999-2020 Andrew Chernow
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

#ifndef LIBPCL__WIN32_H
#define LIBPCL__WIN32_H

#include <pcl/types.h>
#include <pcl/error.h>
#include <pcl/string.h>
#include <windows.h>
#include <shlobj.h>
#include <sddl.h>

#define NT_READ_TO_MODE (FILE_READ_DATA | FILE_READ_EA | FILE_READ_ATTRIBUTES)

#define NT_WRITE_TO_MODE (FILE_WRITE_DATA | \
  FILE_APPEND_DATA | FILE_WRITE_EA | FILE_WRITE_ATTRIBUTES)

#define NT_EXECUTE_TO_MODE (FILE_EXECUTE)

#define UNIX_READ_TO_RIGHTS (NT_READ_TO_MODE | READ_CONTROL | SYNCHRONIZE)

#define UNIX_WRITE_TO_RIGHTS (NT_WRITE_TO_MODE | READ_CONTROL | \
  SYNCHRONIZE | DELETE | FILE_DELETE_CHILD)

#define UNIX_EXECUTE_TO_RIGHTS (NT_EXECUTE_TO_MODE | READ_CONTROL | \
  SYNCHRONIZE)

#define SID_ISGROUP(_sid_type) (_sid_type == SidTypeGroup || \
  _sid_type == SidTypeAlias || _sid_type == SidTypeWellKnownGroup)

#ifdef __cplusplus
extern "C" {
#endif

void ipcl_win32_sd_init(SECURITY_DESCRIPTOR *sd);

int ipcl_win32_sd_create(SECURITY_DESCRIPTOR *sd, const tchar_t *owner,
	const tchar_t *group, mode_t mode);

/** Get the SECURITY_DESCRIPTOR for the given path.
 *
 * @param[in] path
 * @param[out] sd a pointer to a security descriptor pointer to receive an allocated self-relative
 * descriptor for the given path. Caller must free this with pcl_free.
 * @param sdlen
 * @param world_sid optional pointer to an allocated SID that must be passed to FreeSid.
 * @param owner_sid optional pointer to the owner sid. This memory is owned by the security
 * descriptor and must not passed to a free function.
 * @param group_sid optional pointer to the group sid. This memory is owned by the security
 * descriptor and must not passed to a free function.
 * @param have_dacl optional pointer to a BOOL that indicates if the security descriptor
 * has a DACL.
 * @param dacl optional pointer to a DACL. This memory is owned by the security
 * descriptor and must not passed to a free function.
 * @param secinfo bit mask that indicates what security information is desired. See the WinAPI
 * GetKernelObjectSecurity function.
 * @return 0 for success and -1 on error. If this fails, no output arguments are valid.
 */
int ipcl_win32_sd_get(const tchar_t *path, SECURITY_DESCRIPTOR **sd, DWORD *sdlen,
	PSID *world_sid, PSID *owner_sid, PSID *group_sid, BOOL *have_dacl, PACL *dacl,
	SECURITY_INFORMATION secinfo);

/** Clears an "absolute" SECURITY_DESCRIPTOR. Typically this is used as the clear operation for
 * descriptors populated by ipcl_win32_sd_create, but that is not a requirement. This can
 * clear any valid absolute SECURITY_DESCRIPTOR.
 * @param sd pointer to a SECURITY_DESCRIPTOR. Note, this function doesn't free the descriptor
 * pointer itself, just its internals. After this call, the descriptor is empty.
 * @return 0 for success and -1 on error. This will fail if the provided security descriptor
 * is NOT absolute.
 */
int ipcl_win32_sd_clear(SECURITY_DESCRIPTOR *sd);

int ipcl_win32_process_token(HANDLE *htok);

int ipcl_win32_token_into(HANDLE htok, TOKEN_INFORMATION_CLASS type, void **data);

/** Duplicates the given SID
 *
 * @param sid
 * @return
 */
PSID ipcl_win32_sid_dup(PSID sid);

PSID ipcl_win32_name_to_sid(const tchar_t *name, int *type);

int ipcl_win32_sid_to_name(PSID sid, char *out, size_t len);

/** Allocates a new world SID.
 * @return
 */
PSID ipcl_win32_world_sid(void);

/** Convert the user, group or other unix mode bits to Windows NT access rights.
 * @param mode
 * @param type one of S_IRUSR, S_IRGRP or S_IROTH
 * @return
 */
DWORD ipcl_win32_mode_to_rights(mode_t mode, int type);

/** Converts the given NT access rights to user, group or other mode bits.
 * @param rights the ACCESS_ALLOWED_ACE mask to convert
 * @param type one of S_IRUSR, S_IRGRP or S_IROTH
 * @return unix style mode bits
 */
mode_t ipcl_win32_rights_to_mode(DWORD rights, int type);

int ipcl_win32_logon(const tchar_t *username, const tchar_t *domain,
	const tchar_t *password, HANDLE *htok);

/** Get TokenUser SID of process.
 * @return application must free this
 */
PSID ipcl_win32_process_sid(void);

/* free uid/gid when they are provided and are non-NULL on return */
int ipcl_win32_file_security(const tchar_t *path, mode_t *mode, uid_t *uidp, gid_t *gidp);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL__WIN32_H__
