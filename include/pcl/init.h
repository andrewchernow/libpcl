
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

#ifndef LIBPCL_INIT_H
#define LIBPCL_INIT_H

/** @defgroup init Initialize Application
 * Responisble for intializing the PCL library. ::pcl_init must be called before any other
 * PCL function. This is typically accomplished by calling it from within the
 * application's \c main function. This module also includes a \c getopt implementation
 * since calling ::pcl_init and argument parsing are typically both done in \c main.
 * #### Basic Usage
 * @code
 * #include <pcl/init.h>
 * #include <pcl/error.h>
 * #include <pcl/io.h>
 * #include <stdlib.h>
 *
 * int pcl_main(int argc, pchar_t **argv)
 * {
 *   // always call first
 *   pcl_init();
 *
 *   // a requires argument, b has optional argument and c & d have no arguments
 *   const pchar_t *optstr = _P("a:b::cd");
 *
 *   // match the 'val' member of pcl_option_t to the 'optstr' short options (not required)
 *   pcl_option_t options[] = {
 *     {_P("apple"), PclReqArg, _P('a')},
 *     {_P("banana"), PclOptArg, _P('b')},
 *     {_P("cherry"), PclNoArg, _P('c')}
 *     // 'd' is not mapped to a long option
 *   };
 *
 *   // initialize option state
 *   pcl_optstate_t *state = pcl_initopt(argc, argv, optstr, options, countof(options));
 *
 *   if(!state)
 *     PANIC("failed to initialize option state", 0);
 *
 *   int opt;
 *   pchar_t *value;
 *   pcl_option_t *longopt;
 *
 *   // 3rd argument longopt can be NULL, which is common
 *   while((opt = pcl_getopt(state, &value, &longopt)) > 0)
 *   {
 *     pchar_t *longname = longopt ? longopt->name : _P("<short-only>");
 *
 *     switch(opt)
 *     {
 *       case _P('a'):
 *         pcl_printf("a:%Ps] %Ps\n", longname, value);
 *         break;
 *
 *       case _P('b'):
 *       	 pcl_printf("b:%Ps] %Ps\n", longname, value ? value : _P("<no-value>"));
 *       	 break;
 *
 *       case _P('c'):
 *       	 pcl_printf("c:%Ps] set\n", longname);
 *       	 break;
 *
 *       case _P('d'):
 *       default:
 *       	pcl_printf("d:%Ps] set\n", longname); // prints "d:<short-only>] set\n"
 *     }
 *   }
 *
 *   pcl_free(state);
 *
 *   if(opt == -1)
 *     PANIC("getopt failed", 0);
 *
 *   return app_start();
 * }
 * @endcode
 * @{
 */

#include <pcl/types.h>

#ifdef PCL_WINDOWS
#	define pcl_main wmain
#else
#	define pcl_main main
#endif

#ifdef __cplusplus
extern "C" {
#endif

enum pcl_optpolicy
{
	/** Option does not take any arguments */
	PclNoArg,
	/** Option requires an argument */
	PclReqArg,
	/** Option takes an optional argument */
	PclOptArg
};

typedef struct
{
	/** Option name without the dashes. */
	pchar_t *name;

	/** Indicates if the option has no value, optional value or required value.
	 * @see PclNoArg, PclOptArg, PclReqArg
	 */
	int policy;

	/** A value, like a character, that can be used to identify the long option. This is simpler
	 * to work with (like a switch) then comparing long option names. It is common to set this
	 * value to the cooresponding short option.
	 * @warning This value should not be -1 or 0, as these are ::pcl_getopt return values used to,
	 * respectively, indicate an error or completion.
	 */
	int val;
} pcl_option_t;

/** State management for argument parsing.
 * @see pcl_initopt, pcl_getopt
 */
typedef struct tag_pcl_optstate pcl_optstate_t;

/** Initializes the PCL library. This must be called before using the PCL library. */
PCL_EXPORT void pcl_init(void);

/** Intialize command-line argument parser. This behaves very similar to POSIX \c getopt_long.
 * Some differences are
 *  * This doesn't permute (rearrange) arguments
 *  * No support for \c '+' or \c POSIXLY_CORRECT
 *  * opaque pcl_optstate_t manages state rather than global variables
 *  * returns 0 when complete, not -1. -1 indicates an error
 *  * ? never returned to indicate an error, -1 is always returned
 *  * this never outputs to stderr, it instead returns -1 and set pcl_errno
 *
 * @warning If the short options \a optstr defines different argument policies than the
 * matching pcl_option_t.val, the behavior is undefined. If time is taken to map short => long,
 * make sure policies match: ::PclNoArg, ::PclReqArg or ::PclOptArg.
 *
 * @param argc argument count, typically from \c main
 * @param argv pointer to an array of arguments, typically from \c main
 * @param optstr option characters. If a character is followed by a colon, the option requires
 * an argument. If followed by two colons, the option takes an optional argument. In either case,
 * the argument can be specified as: \c "-Ovalue", \c "-O=value" or \c "-O value". When dealing
 * with an optional argument, the 3rd form excludes the value if it begins with a '-': ie.
 * \c "-O -value". This is not the case for a required argument. This can be \c NULL as long as
 * the \a options parameter is not \c NULL.
 * @param options array of option structures defining legitimate long options. The option
 * structure defines an option \a name, option argument \a policy and a \a val used as the
 * return value when a long option is found. Typically, \a val is set to its corresponding short
 * option character, but this is not a requirement. The argument can be specified as
 * \c "--long=value" or \c "--long value". As with short options, the 2nd form excludes optional
 * arguments that begin with a '-': ie. \c "--long -value". This is not the case for a required
 * argument. This can be \c NULL as long as the \a optstr parameter is not \c NULL.
 * @param num_options number of option structures in the \a options parameter.
 * @return pointer to an option state structure, that must be freed by caller, or \c NULL on error.
 * An error only occurs if there is an invalid argument.
 */
PCL_EXPORT pcl_optstate_t *pcl_initopt(int argc, pchar_t **argv, const pchar_t *optstr,
	pcl_option_t *options, int num_options);

/** Parse command-line arguments.
 * @param state pointer to a getopt state structure intialized via ::pcl_initopt
 * @param value pointer to a string to receive the value, if any. This can be \c NULL. If
 * this is \c NULL, there is no way to get an option's argument value. This value must be
 * copied to persist after argument parsing, since it is direct pointer to the \c argv array
 * provided during ::pcl_initopt.
 * @param longopt pointer to an option to receive the matching option object. The comparison
 * works for long or short option parsing. When a short option is found with a matching
 * pcl_option_t.val value, that option object is written here. For long options, the comparison
 * is based off pcl_option_t.name. If a short option has no matching \a val within long option
 * objects, or they were not provided to ::pcl_initopt, then this is set to \c NULL.
 * This is an optional parameter and can be \c NULL.
 * @return The short option character or long option pcl_option_t.val. When all arguments
 * have been parsed, this returns 0. On error, -1 is returned.
 */
PCL_EXPORT int pcl_getopt(pcl_optstate_t *state, pchar_t **value, pcl_option_t **longopt);

#ifdef __cplusplus
}
#endif

/** @} */
#endif //LIBPCL_INIT_H


