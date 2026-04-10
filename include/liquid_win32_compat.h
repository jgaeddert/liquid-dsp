/*
 * Windows compatibility layer for liquid-dsp
 * Provides POSIX-like functions for Windows platforms
 *
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 * MIT License
 */
#ifndef __LIQUID_WIN32_COMPAT_H__
#define __LIQUID_WIN32_COMPAT_H__

#ifdef _WIN32

/* Include windows.h early for MSVC builds */
#ifdef _MSC_VER
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#endif

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * sys/resource.h compatibility
 * Provides struct rusage and getrusage() using Windows API
 */

/* Timing structure compatible with POSIX timeval */
struct liquid_timeval {
	long tv_sec;	/* seconds */
	long tv_usec;	/* microseconds */
};

/* Resource usage structure compatible with POSIX rusage */
struct rusage {
	struct liquid_timeval ru_utime;	/* user time used */
	struct liquid_timeval ru_stime;	/* system time used */
	/* Other fields from POSIX are not used by liquid-dsp */
};

#define RUSAGE_SELF 0

/* Get resource usage - Windows implementation */
static inline int getrusage(int who, struct rusage *usage)
{
	if (who != RUSAGE_SELF || usage == NULL)
		return -1;

#ifdef _MSC_VER
	/* MSVC: Use Windows API */
	{
		FILETIME creation_time, exit_time, kernel_time, user_time;
		ULARGE_INTEGER kernel, user;

		if (!GetProcessTimes(GetCurrentProcess(),
				     &creation_time, &exit_time,
				     &kernel_time, &user_time)) {
			return -1;
		}

		/* Convert FILETIME (100-nanosecond intervals) to timeval */
		kernel.LowPart = kernel_time.dwLowDateTime;
		kernel.HighPart = kernel_time.dwHighDateTime;
		user.LowPart = user_time.dwLowDateTime;
		user.HighPart = user_time.dwHighDateTime;

		/* FILETIME is in 100-ns units, convert to seconds and microseconds */
		usage->ru_stime.tv_sec = (long)(kernel.QuadPart / 10000000ULL);
		usage->ru_stime.tv_usec = (long)((kernel.QuadPart % 10000000ULL) / 10);
		usage->ru_utime.tv_sec = (long)(user.QuadPart / 10000000ULL);
		usage->ru_utime.tv_usec = (long)((user.QuadPart % 10000000ULL) / 10);
	}
#else
	/* MinGW: Use clock() as fallback */
	{
		clock_t t = clock();
		usage->ru_utime.tv_sec = (long)(t / CLOCKS_PER_SEC);
		usage->ru_utime.tv_usec = (long)((t % CLOCKS_PER_SEC) *
						 (1000000 / CLOCKS_PER_SEC));
		usage->ru_stime.tv_sec = 0;
		usage->ru_stime.tv_usec = 0;
	}
#endif
	return 0;
}

/*
 * getopt.h compatibility for MSVC
 * MinGW has getopt.h, so this is only needed for MSVC
 */
#ifdef _MSC_VER

/* Import/export macros for DLL */
#ifdef LIQUID_BUILD_DLL
#define LIQUID_GETOPT_API __declspec(dllexport)
#else
#define LIQUID_GETOPT_API __declspec(dllimport)
#endif

LIQUID_GETOPT_API extern char *optarg;
LIQUID_GETOPT_API extern int optind, opterr, optopt;

struct option {
	const char *name;	/* name of long option */
	int has_arg;		/* whether option takes argument */
	int *flag;		/* if not NULL, set *flag to val when found */
	int val;		/* value to return or store */
};

#define no_argument 0
#define required_argument 1
#define optional_argument 2

/* Simple getopt implementation for MSVC */
int getopt(int argc, char * const argv[], const char *optstring);
int getopt_long(int argc, char * const argv[],
		const char *optstring,
		const struct option *longopts, int *longindex);

#endif /* _MSC_VER */

#ifdef __cplusplus
}
#endif

#endif /* _WIN32 */

#endif /* __LIQUID_WIN32_COMPAT_H__ */
