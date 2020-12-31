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

#ifdef PCL_WINDOWS
#	include <windows.h>
#else
#	include <dirent.h>
#	include <unistd.h>
#	include <dlfcn.h>
#endif

#include <pcl/init.h>
#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#define SKIPWS(_s) while(isspace(*(_s))) (_s)++

static int num_suites = 0;
static int num_tests = 0;
static int num_failed = 0;

typedef bool (*casefunc_t)(void);

static casefunc_t get_case(const char *name)
{
	static void *handle;
	char tcname[512];

	strcpy(tcname, "testcase_");
	strcat(tcname, name);

#ifdef PCL_WINDOWS
	if(!handle)
		handle = GetModuleHandle(NULL);
	return (casefunc_t) GetProcAddress(handle, tcname);
#else
	if(!handle)
		handle = dlopen(NULL, RTLD_LOCAL | RTLD_LAZY);
	return (casefunc_t) dlsym(handle, tcname);
#endif
}

static char *wrap_summary(const char *summary)
{
	size_t len = strlen(summary);
	char *wrapped_summary = malloc(len + ((size_t) (len / 80) * 12) + 1);
	char *dst = wrapped_summary;

	for(int line_len = 1; *summary; summary++, line_len++)
	{
		if(*summary == ' ' && line_len >= 80)
		{
			memcpy(dst, "\n           ", 12);
			dst += 12;
			line_len = 0;
		}
		else
		{
			*dst++ = *summary;
		}
	}

	*dst = 0;
	return wrapped_summary;
}

static void run_case(const char *name, const char *summary)
{
	printf("  Case: %s\n", name);

	num_tests++;

	if(summary)
	{
		char *wrapped_summary = wrap_summary(summary);
		printf("  Summary: %s\n", wrapped_summary);
		free(wrapped_summary);
	}

	casefunc_t casefunc = get_case(name);

	if(!casefunc)
	{
		fprintf(stderr, "  Status: \x1B[31mFailed\x1B[0m - cannot find test case '%s', "
										"try recompiling test target\n", name);
		num_failed++;
		return;
	}

	printf("  Status: Running");
	fflush(stdout);

	bool success = casefunc();

	if(!success)
		num_failed++;

	const char *status = success ? "Success" : "Failed";
	int color = success ? 32 : 31; // green or red

	printf("\r                       \r  Status: \x1B[%dm%s\x1B[0m\n\n", color, status);
}

static void run_suite(const char *suite)
{
	size_t len = strlen(suite);

	/* skip any none ".c" file and "test.c" */
	if(len <= 2 || strcmp(suite + len - 2, ".c") != 0 || strcmp(suite, "test.c") == 0)
		return;

	char *p = strrchr(suite, '.');
	printf("Suite: %.*s\n", (int) (p - suite), suite);

	bool in_summary = false;
	char *line, buf[1024], *summary = NULL;
	FILE *fp = fopen(suite, "r");

	if(!fp)
	{
		fprintf(stderr, "error opening suite file: '%s'\n", suite);
		exit(1);
	}

	num_suites++;

	while((line = fgets(buf, sizeof(buf), fp)))
	{
		p = strrchr(line, '\n');

		if(p)
			*p = 0;

		if(*--p == '\r')
			*p = 0;

		SKIPWS(line);

		if(in_summary)
		{
			p = strstr(line, "*/");

			if(p)
				*p = 0;

			if(*line == '*')
			{
				line++;
				SKIPWS(line);
			}

			size_t n2 = strlen(line);

			if(n2 > 0)
			{
				size_t n1 = strlen(summary);
				summary = realloc(summary, n1 + 1 + n2 + 1); // add 1 for space and NUL
				summary[n1++] = ' ';
				memcpy(summary + n1, line, n2 + 1);
			}

			in_summary = p == NULL;
		}
		else if(strncmp(line, "/**$", 4) == 0)
		{
			line += 4;
			SKIPWS(line);

			p = strstr(line, "*/");

			if(p)
				*p = 0;

			if(*line)
				summary = strdup(line);

			in_summary = p == NULL;
		}
			/* TESTCASE(name) account for possible spacing around ( and ) */
		else if(strncmp(line, "TESTCASE", 8) == 0)
		{
			char *name = line + 8;

			SKIPWS(name);

			if(*name++ != '(')
				continue;

			SKIPWS(name);

			for(p = name; *p && !isspace(*p) && *p != ')'; p++);

			if(!*p)
				continue;

			*p = 0;

			run_case(name, summary);

			if(summary)
			{
				free(summary);
				summary = NULL;
			}
		}
	}

	fclose(fp);
}

static void find_suites(void)
{
#ifdef PCL_WINDOWS
	WIN32_FIND_DATAA wfd;

	HANDLE hfind = FindFirstFileA(".\\*", &wfd);

	if(hfind == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "cannot list tests directory\n");
		exit(1);
	}

	do
		if((wfd.dwFileAttributes & (FILE_ATTRIBUTE_DIRECTORY | FILE_ATTRIBUTE_REPARSE_POINT)) == 0)
			run_suite(wfd.cFileName);
	while(FindNextFileA(hfind, &wfd));

	FindClose(hfind);
#else
	DIR *dp = opendir(".");

	if(!dp)
	{
		fprintf(stderr, "cannot list tests directory\n");
		exit(1);
	}

	struct dirent *ent;

	while((ent = readdir(dp)))
		if(ent->d_type == DT_REG)
			run_suite(ent->d_name);

	closedir(dp);
#endif
}

int main(int argc, char **argv)
{
	(void) argc;
	(void) argv;

	pcl_init();

#ifdef PCL_WINDOWS
	SetCurrentDirectoryA("tests");
#else
	chdir("tests");
#endif

	find_suites();

	printf("Report\n  Suites: %d\n  Tests: %d\n  Failed: %d\n", num_suites, num_tests, num_failed);

	return 0;
}
