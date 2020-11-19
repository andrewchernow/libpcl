# PCL Event System

The PCL event system allows applications to dispatch events to registered handlers. By
default, PCL includes a handful of internal handlers designed to receive two built-in events:
`PCL_EVENT_INIT` and `PCL_EVENT_THREADINIT`. `PCL_EVENT_INIT` is dispatched by the `pcl_init`
function after all intialization code has completed: so just before returning. The
`PCL_EVENT_THREADINIT` event is dispatched when a PCL thread is created via `pcl_thread_create`.
If `pcl_thread_create` is not used to create a thread, one can manually dispatch the init event
from within the thread:

```c
pcl_event_dispatch(PCL_EVENT_THREADINIT, optional_event_data)
``` 

This is important since many of the built-in handlers require per thread initialization. Note 
that PCL events are dispatched to the thread that called `pcl_event_dispatch`.

## Application-specific Events

An application can define their own events, which is common as the built-in events are really
designed for application and thread initialization. Defining application events merely requires
chosing an event identifier. PCL defines `PCL_EVENT_USERBASE` that should be used as base
identifier for all application defined events.

```c
#define APP_EVENT_ABC (PCL_EVENT_USERBASE + 0)
#define APP_EVENT_DEF (PCL_EVENT_USERBASE + 1)
```
The above identifiers can be passed to `pcl_event_dispatch`. All registered event handlers will
receive the event and can elect to perform processing upon receipt or do nothing. The built-in
PCL event handlers only respond to `PCL_EVENT_INIT` and `PCL_EVENT_THREADINIT`.




