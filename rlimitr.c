/*
 * rlimitr - get/set resource limits via setrlimit(3)
 * Copyright (C) 2010 Hauke Henningsen
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * 2 simple versions to compile this stuff:
 * gcc -fPIC -O2 -o rlimitr rlimitr.c
 * gcc -fPIC -O2 -shared -DRLIMITR_LIBRARY_ONLY -o rlimitr.so rlimitr.c
 */

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <sys/time.h>
#include <sys/resource.h>

#define RLIMITR_VERSION "0.1.0"

#ifndef __GNUC__
#define __attribute__()
#endif

struct rlimitr_entry {
	int res;
	const char* name;
};

static const char* rlim_infty_name = "RLIM_INFINITY";

static void rlimitr_set(struct rlimitr_entry* entry, const char* value) {
	struct rlimit res;
	int rv, n, m;
	int hardlimit = 0;
	int mode = 0;
	rlim_t v;
	char* eptr = NULL;
	char curstr[22], maxstr[22];
	rlim_t* change_ptr;

	rv = getrlimit(entry->res, &res);
	if (rv == -1) {
		perror("getrlimit()");
		return;
	}

	if (value == NULL) { /* print value only */
#define RLIM_TO_CHAR_ARR(arr, v) \
		if (v == RLIM_INFINITY) strcpy(arr, rlim_infty_name); \
		else snprintf(arr, sizeof(arr), "0x%lx", (long)v); \

		RLIM_TO_CHAR_ARR(curstr, res.rlim_cur)
		RLIM_TO_CHAR_ARR(maxstr, res.rlim_max)

		printf("%s=s%s:h%s\n", entry->name, curstr, maxstr);
		return;
	}

	switch(value[0]) {
		case 'h':
			++value;
			hardlimit = 1;
			break;
		case 's':
			++value;
			hardlimit = 0; /* it should already be set to 0... set it anyway. */
			break;
		default:
			break;
	}
	switch(value[0]) {
		case '+':
			++value;
			mode = 1;
			break;
		case '-':
			++value;
			mode = -1;
			break;
		default:
			break;
	}

	eptr = strchr(value, ':');
	
	assert(eptr >= value);
	if (strncmp(value, rlim_infty_name, eptr ? (size_t)(eptr-value) : strlen(value)) == 0) {
		v = RLIM_INFINITY;
	} else {
		v = strtol(value, &eptr, 0);
		if (eptr) {
			/* this format is inspired by ls(1) */
			n = 0;
			m = 1024;
			switch (toupper(*eptr)) {
				case 'K':
					n = 1; break;
				case 'M':
					n = 2; break;
				case 'G':
					n = 3; break;
				case 'T':
					n = 4; break; /* we should get probles with long here already... hmpf, not our problem, really */
				case 'P':
					n = 5; break;
				case 'E':
					n = 6; break;
				case 'Z':
					n = 7; break;
				case 'Y':
					n = 8; break;
				default:
					--eptr;
			}

			if (*++eptr == 'B') m = 1000, ++eptr;
			while (n--) v *= m;
		}
	}

	if (eptr == value) {
		fprintf(stderr, "Warning: bad string found where value was expected!\n");
		return;
	}

	if (hardlimit) {
		change_ptr = &res.rlim_max;
	} else {
		change_ptr = &res.rlim_cur;
	}

	if (mode != 0 && *change_ptr == RLIM_INFINITY) {
		fprintf(stderr, "Warning: Doing arithmetics on RLIM_INFINITY is a bad idea(tm)!\n");
	}

	switch (mode) {
		case 0:
			*change_ptr = v;
			break;
		case 1:
			*change_ptr += v;
			break;
		case -1:
			*change_ptr -= v;
			break;
	}

	rv = setrlimit(entry->res, &res);
	if (rv == -1) {
		perror("setrlimit()");
	}

	if (!eptr) return;
	switch (*eptr) {
		case '\0':
			break;
		case ':':
			++eptr;
			rlimitr_set(entry, eptr);
			break;
		default:
			fprintf(stderr, "Warning: unrecognized character %c in value!\n", *eptr);
			break;
	}
}

#define __R(n) (struct rlimitr_entry){ n, #n }
static struct rlimitr_entry entries[] = {
#ifdef RLIMIT_AS
	__R(RLIMIT_AS),
#endif
#ifdef RLIMIT_CORE
	__R(RLIMIT_CORE),
#endif
#ifdef RLIMIT_CPU
	__R(RLIMIT_CPU),
#endif
#ifdef RLIMIT_DATA
	__R(RLIMIT_DATA),
#endif
#ifdef RLIMIT_FSIZE
	__R(RLIMIT_FSIZE),
#endif
#ifdef RLIMIT_LOCKS
	__R(RLIMIT_LOCKS),
#endif
#ifdef RLIMIT_MEMLOCK
	__R(RLIMIT_MEMLOCK),
#endif
#ifdef RLIMIT_MSGQUEUE
	__R(RLIMIT_MSGQUEUE),
#endif
#ifdef RLIMIT_NICE
	__R(RLIMIT_NICE),
#endif
#ifdef RLIMIT_NOFILE
	__R(RLIMIT_NOFILE),
#endif
#ifdef RLIMIT_NPROC
	__R(RLIMIT_NPROC),
#endif
#ifdef RLIMIT_RSS
	__R(RLIMIT_RSS),
#endif
#ifdef RLIMIT_RTPRIO
	__R(RLIMIT_RTPRIO),
#endif
#ifdef RLIMIT_RTTIME
	__R(RLIMIT_RTTIME),
#endif
#ifdef RLIMIT_SIGPENDING
	__R(RLIMIT_SIGPENDING),
#endif
#ifdef RLIMIT_STACK
	__R(RLIMIT_STACK),
#endif
/* from setrlimit(2freebsd) */
#ifdef RLIMIT_SBSIZE
	__R(RLIMIT_SBSIZE),
#endif
#ifdef RLIMIT_SWAP
	__R(RLIMIT_SWAP),
#endif
#ifdef RLIMIT_NTPS
	__R(RLIMIT_NTPS),
#endif
	(struct rlimitr_entry){0, NULL}
};

#ifndef RLIMITR_DIRECT_EXECUTABLE_ONLY
#ifdef __GNUC__
__attribute__((constructor))
void rlimitr_init () {
#else
void _init() {
#endif
	struct rlimitr_entry* it;
	for (it = entries; it->name; ++it) {
		char* env = getenv(it->name);
		if (!env)
			continue;

		rlimitr_set(it, env);
	}
}
#endif

#ifndef RLIMITR_LIBRARY_ONLY
static int print_limits() {
	struct rlimitr_entry* it;
	for (it = entries; it->name; ++it)
		rlimitr_set(it, NULL);
	return 0;
}

__attribute__((weak))
int main(int argc, char** argv) {
	int i;
	char* eq;

	if (argc > 1 && strcmp(argv[1], "--help") == 0) {
		printf(
"rlimitr -- set/get resource limits\n"
"\n"
"Usage:\n"
"\trlimitr [NAME=[VALUE]] ... [command [args] ...]\n"
"\trlimitr [--help|--version]\n"
"\n"
"The command line environment is actually inspired a bit by env(1),\n"
"so you can usually use it very similarily to env.\n"
"\tNAME=[VALUE]     Set the resource limit identified by NAME to VALUE;\n"
"\t                 for possible formats for VALUE see below.\n"
"\t                 If VALUE is omitted, just print the current value\n"
"\t                 to stdout (in a format, that, again, could be\n"
"\t                 used by rlimitr).\n"
"\n"
"\tcommand [args]   A command to be executed (using a simple execvp(3)).\n"
"\n"
"\t--version        Output information about rlimitr and exit.\n"
"\t--help           Output what you are currently reading.\n"
"If no arguments are given, simply print out all current settings.\n"
"\n"
"VALUE format:\n"
"[s|h][+|-]number\n"
"\t* If the argument starts with 's', change the soft limit (default).\n"
"\t* If the argument starts with 'h', change the hard limit.\n"
"\t  (Multiple VALUEs can be seperated with ':', for example\n"
"\t  if you want to set both limits.)\n"
"\t* '+' indicates that the limit should be incremented by this value.\n"
"\t* '-' indicates decrementing the limit.\n"
"\t* If none of '+' or '-' is given, the argument will be interpreted\n"
"\t  as an absolute value.\n"
"\t* number may be any decimal number, an octal starting by '0'\n"
"\t  or a hexadecimal value starting with '0x'\n"
"\t  (using strtol(3) with 0 as the base argument).\n"
"\t  Suffixes to number may be K, KB, M, MB, G etc., respectively\n"
"\t  representing multiplication with 1024, 1000, 1024**2, 1000**2,\n"
"\t  1024**3. (inspired by the ls(1) format)\n"
"\t* As a special value, RLIM_INFINITY represents the biggest value\n"
"\t  the limit may take (or, basically, whatever the\n"
"\t  setrlimit(3)/getrlimit(3) manpage tells you).\n"
"\n"
"Example:\n"
"\trlimitr RLIMIT_AS=s50M:h50M ls\n"
"\t  runs ls with a maximum address space of 50 MB (0x3200000 bytes)\n"
"\t  (whyever you should want to do this to poor ls... ;-))\n"
"Also you may choose to give arguments to rlimitr via the environment\n"
"for whatever reason you want. (So, the above would essentially be\n"
"equivalent to `env RLIMIT_AS=s50M:h50M rlimitr ls`.)\n"
"For anything else you should probably consult the manpage(s) mentioned above.\n"
		);
		return 0;
	} else if (argc > 1 && strcmp(argv[1], "--version") == 0) {
		printf(
"rlimitr " RLIMITR_VERSION "\n"
"Copyright (C) 2010 Hauke Henningsen\n"
"License GPLv3+: GNU GPL version 3 or later <http://gnu.org/licenses/gpl.html>.\n"
"This is free software: you are free to change and redistribute it.\n"
"There is NO WARRANTY, to the extent permitted by law.\n"
		);
		return 0;
	}

	for (i = 1; i < argc; ++i) {
		eq = strchr(argv[i], '=');
		if (!eq) {
			execvp(argv[i], argv+i);
			perror("execvp()");
			return 1;
		}

		*(eq++) = '\0';

		struct rlimitr_entry* it;
		for (it = entries; it->name; ++it) {
			if (strcmp(argv[i], it->name) == 0)
				break;
		}
		if (!it->name) { /* failed to find any */
			fprintf(stderr, "No such resource name: %s\n", argv[i]);
			return 2;
		}

		if (*eq == '\0')
			eq = NULL;

		rlimitr_set(it, eq);
	}

	return print_limits();
}
#endif
