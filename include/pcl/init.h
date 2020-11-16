
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

#ifndef LIBPCL_INIT_H
#define LIBPCL_INIT_H

#ifdef __cplusplus
extern "C" {
#endif

/** Structure used for initializing the PCL library.
 * @member argc number of arguments in argv member, ignored if register_handlers is NULL
 * @member argv tchar_t string array from main(), use pcl_tmain instead of main. Ignored if
 * register_handlers is NULL
 * @member userp application-specific data, ignored if register_handlers is NULL
 * @member register_handlers called by pcl_init to allow the application to register
 * handlers prior to a PCL_EVENT_INIT event.
 */
typedef struct tag_pcl_init pcl_init_t;

struct tag_pcl_init
{
	int argc;
	tchar_t **argv;
	void *userp;
	void (*register_handlers)(pcl_init_t *init);
};

/** Initializes the PCL library. This must be called before using the PCL library.
 *
 * @param init optional argument that if supplied, will call the pcl_init_t.register_handlers
 * function, allowing applications to register their event handlers before pcl_init dispatches
 * a PCL_EVENT_INIT. If that is not required, provide NULL for this argument. Optionally, the
 * application can set the argc and argv members of pcl_init_t so they can be used during
 * register_handlers callback. In addition, pcl_init_t has a "void *userp" member that can
 * be set to any application-specific data structure.
 *
 * @code
 * // First Example: demonstrates utilizing pcl_init_t
 *
 * #include &lt;pcl/init.h&gt;
 * #include &lt;pcl/event.h&gt;
 * #include &lt;pcl/tchar.h&gt;
 *
 * static void my_register_handlers(pcl_init_t *init)
 * {
 *   // some_feature_handler will receive a PCL_EVENT_INIT event
 *   if(pcl_tcscmp(init->argv[1], _T("some-feature")) == 0)
 *     pcl_event_register(some_feature_handler);
 * }
 *
 * int pcl_tmain(int argc, tchar_t **argv)
 * {
 *   pcl_init_t init = {
 *     .argc = argc,
 *     .argv = argv,
 *     .userp = NULL,
 *     .register_handlers = my_register_handlers
 *   };
 *
 *   pcl_init(&init);
 *   app_start();
 * }
 *
 * // 2nd Example: no pcl_init_t and manually register handlers
 *
 * #include &lt;pcl/init.h&gt;
 * #include &lt;pcl/tchar.h&gt;
 *
 * int pcl_main(int argc, tchar_t **argv)
 * {
 *   pcl_init(NULL);
 *   register_my_handlers_now(argc, argv); // never gets a PCL_EVENT_INIT
 *   app_start();
 * }
 * @endcode
 */
PCL_EXPORT void pcl_init(pcl_init_t *init);

#ifdef __cplusplus
}
#endif

#endif //LIBPCL_INIT_H


