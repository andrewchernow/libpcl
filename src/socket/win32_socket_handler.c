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

#include "_socket.h"
#include <pcl/error.h>
#include <pcl/event.h>
#include <stdlib.h>

void
ipcl_win32_socket_handler(uint32_t which, void *data)
{
	UNUSED(data);

	if(which != PCL_EVENT_INIT)
		return;

	bool found = false;
	WORD versions[5] = {
		MAKEWORD(2, 2), MAKEWORD(2, 1), MAKEWORD(2, 0), MAKEWORD(1, 1), MAKEWORD(1, 0)
	};

	for(int i = 0; i < countof(versions); i++)
	{
		WSADATA wsa;
		WORD ver = versions[i];
		int err = WSAStartup(ver, &wsa);

		if(err == 0)
		{
			found = LOBYTE(wsa.wVersion) == LOBYTE(ver) && HIBYTE(wsa.wVersion) == HIBYTE(ver);
			if(found)
				break; /* good to go */
		}
		/* any other error is a show stopper. WSASYSNOTREADY or WSAEPROCLIM. */
		else if(err != WSAVERNOTSUPPORTED)
		{
			char msg[512];

			pcl_err_osmsg(err, msg, sizeof(msg));
			fprintf(stderr, "Winsock failed to load: %s[%d] - %s\n", pcl_err_osname(err), err, msg);

			exit(1);
		}
	}

	if(!found)
	{
		fprintf(stderr, "Could not load Winsock: tried 2.2, 2.1, 2.0, 1.1 and 1.0\n");
		exit(1);
	}
}
