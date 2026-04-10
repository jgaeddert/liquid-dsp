/*
 * Minimal getopt implementation for MSVC
 * Based on public domain implementations
 *
 * Copyright (c) 2007 - 2025 Joseph Gaeddert
 * MIT License
 */

#if defined(_MSC_VER) && defined(_WIN32)

#include <stdio.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

char *optarg = NULL;
int optind = 1;
int opterr = 1;
int optopt = '?';

/* Reset getopt state for new parsing */
void getopt_reset(void)
{
	optind = 1;
	optarg = NULL;
}

int getopt(int argc, char * const argv[], const char *optstring)
{
	static int sp = 1;
	int c;
	const char *cp;

	if (sp == 1) {
		if (optind >= argc ||
		    argv[optind][0] != '-' ||
		    argv[optind][1] == '\0') {
			return -1;
		} else if (strcmp(argv[optind], "--") == 0) {
			optind++;
			return -1;
		}
	}

	optopt = c = argv[optind][sp];
	cp = strchr(optstring, c);

	if (c == ':' || cp == NULL) {
		if (opterr)
			fprintf(stderr, "%s: illegal option -- %c\n",
				argv[0], c);
		if (argv[optind][++sp] == '\0') {
			optind++;
			sp = 1;
		}
		return '?';
	}

	if (*++cp == ':') {
		if (argv[optind][sp + 1] != '\0') {
			optarg = &argv[optind++][sp + 1];
		} else if (++optind >= argc) {
			if (opterr)
				fprintf(stderr,
					"%s: option requires an argument -- %c\n",
					argv[0], c);
			sp = 1;
			return '?';
		} else {
			optarg = argv[optind++];
		}
		sp = 1;
	} else {
		if (argv[optind][++sp] == '\0') {
			sp = 1;
			optind++;
		}
		optarg = NULL;
	}

	return c;
}

struct option {
	const char *name;
	int has_arg;
	int *flag;
	int val;
};

#ifndef no_argument
#define no_argument 0
#define required_argument 1
#define optional_argument 2
#endif

int getopt_long(int argc, char * const argv[],
		const char *optstring,
		const struct option *longopts, int *longindex)
{
	static int sp = 1;
	int i;

	if (sp == 1) {
		if (optind >= argc || argv[optind][0] != '-') {
			return -1;
		}

		/* Handle -- terminator */
		if (strcmp(argv[optind], "--") == 0) {
			optind++;
			return -1;
		}

		/* Check for long option */
		if (argv[optind][1] == '-' && argv[optind][2] != '\0') {
			char *arg = &argv[optind][2];
			char *eq = strchr(arg, '=');
			size_t len = eq ? (size_t)(eq - arg) : strlen(arg);

			for (i = 0; longopts && longopts[i].name; i++) {
				if (strncmp(arg, longopts[i].name, len) == 0 &&
				    longopts[i].name[len] == '\0') {
					if (longindex)
						*longindex = i;

					if (longopts[i].has_arg == required_argument) {
						if (eq) {
							optarg = eq + 1;
						} else if (optind + 1 < argc) {
							optind++;
							optarg = argv[optind];
						} else {
							if (opterr)
								fprintf(stderr,
									"%s: option requires argument -- %s\n",
									argv[0], longopts[i].name);
							optind++;
							return '?';
						}
					} else if (longopts[i].has_arg == optional_argument) {
						optarg = eq ? eq + 1 : NULL;
					} else {
						optarg = NULL;
					}

					optind++;

					if (longopts[i].flag) {
						*longopts[i].flag = longopts[i].val;
						return 0;
					}
					return longopts[i].val;
				}
			}

			if (opterr)
				fprintf(stderr, "%s: unrecognized option '--%s'\n",
					argv[0], arg);
			optind++;
			return '?';
		}
	}

	/* Fall back to short option parsing */
	return getopt(argc, argv, optstring);
}

#ifdef __cplusplus
}
#endif

#endif /* _MSC_VER && _WIN32 */
