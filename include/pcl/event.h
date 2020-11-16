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

#ifndef LIBPCL_EVENT_H__
#define LIBPCL_EVENT_H__

#include <stdint.h>

/** Passed to a pcl_event_handler_t function to indicate that the PCL library is being initialized.
 * @internal only used by builtin event handlers
 * @see PCL_EVENT_POSTINIT
 */
#define PCL_EVENT_INIT 0

/** Passed to a pcl_event_handler_t function to indicate that a thread is being initialized. This is
 * automatically triggered by pcl_thread_create. If a thread is created by other means, call
 * pcl_event_dispatch manually within the thread start routine.
 * @see pcl_init_thread
 */
#define PCL_EVENT_THREADINIT 1

/** Manually dispatched by applications after they have registered their event handlers.
 * This doesn't have to be used. An application is free to define any event identifier
 * to intialize their application.
 *
 * The typical sequence is:
 * @code
 * #include &lt;pcl/init.h&gt;
 * #include &lt;pcl/event.h&gt;
 * #include &lt;pcl/tchar.h&gt;
 *
 * // example defining application-specific events
 * #define MYAPP_EVENT_INIT_DBPOOL (PCL_EVENT_USERBASE + 0)
 *
 * int pcl_main(int argc, tchar_t **argv)
 * {
 *   void *event_data = parse_arguments(argc, argv);
 *
 *   pcl_init(NULL);
 *
 *   pcl_event_register(my_handler);
 *   pcl_event_register(my_hanlder2);
 *   pcl_event_dispatch(PCL_EVENT_POSTINIT, event_data);
 *
 *   // dispatch application-specific event
 *   pcl_event_dispatch(MYAPP_EVENT_INIT_DBPOOL, event_data);
 * }
 * @endcode
 */
#define PCL_EVENT_POSTINT 2

/** User-defined events should be greater than or equal to this value */
#define PCL_EVENT_USERBASE 100

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*pcl_event_handler_t)(uint32_t event, void *data);

/** Dispatches an event to the calling thread.
 * @note dispatching is a per-thread synchronous operation. This function won't return until
 * all event handlers have completed.
 * @param event event id to dispatch. The internal event_handlers ignore all events except
 * PCL_EVENT_INIT and PCL_EVENT_THREADINIT. However, applications can create custom events
 * and reaspond accordingly.
 * @param data event data. PCL_EVENT_INIT and PCL_EVENT_THREADINIT do not use event data
 */
PCL_EXPORT void pcl_event_dispatch(uint32_t event, void *data);

/** Registers a new event handler.
 * @param handler event handler
 */
PCL_EXPORT void pcl_event_register(pcl_event_handler_t handler);

/** Unregisters a previously registered event handler.
 * @param handler event handler
 */
PCL_EXPORT void pcl_event_unregister(pcl_event_handler_t handler);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_EVENT_H__
