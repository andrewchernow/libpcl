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

#ifndef LIBPCL_EVENT_H__
#define LIBPCL_EVENT_H__

/** @defgroup event Events
 * Definitions for managing synchronous events.
 * @{
 */
#include <stdint.h>

/** Passed to a pcl_event_handler_t function to indicate that the PCL library is being initialized.
 * @internal only used by builtin event handlers
 */
#define PCL_EVENT_INIT 0

/** Passed to a pcl_event_handler_t function to indicate that a thread is being initialized. This
 * is automatically triggered by pcl_thread. If a thread is created by other means, call
 * pcl_event_dispatch manually within the thread start routine.
 * @see pcl_init_thread
 */
#define PCL_EVENT_THREADINIT 1

/** User-defined events should be greater than or equal to this value */
#define PCL_EVENT_USERBASE 10

#ifdef __cplusplus
extern "C" {
#endif

/** Event handler prototype.
 * @param event the event identifier
 * @param data optional pointer to event data
 */
typedef void (*pcl_event_handler_t)(uint32_t event, void *data);

/** Dispatches an event to the calling thread.
 * @note dispatching is a per-thread synchronous operation. This function won't return until
 * all event handlers have completed.
 * @param event event id to dispatch. The internal event_handlers ignore all events except
 * PCL_EVENT_INIT and PCL_EVENT_THREADINIT. However, applications can create custom events
 * and respond accordingly.
 * @param data event data. PCL_EVENT_INIT and PCL_EVENT_THREADINIT do not use event data
 */
PCL_PUBLIC void pcl_event_dispatch(uint32_t event, void *data);

/** Registers a new event handler.
 * @param handler event handler
 */
PCL_PUBLIC void pcl_event_register(pcl_event_handler_t handler);

/** Unregisters a previously registered event handler.
 * @param handler event handler
 */
PCL_PUBLIC void pcl_event_unregister(pcl_event_handler_t handler);

#ifdef __cplusplus
}
#endif

/** @} */
#endif //LIBPCL_EVENT_H__
