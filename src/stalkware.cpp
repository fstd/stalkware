/* stalkware.cpp - stalkware(1) launcher
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <err.h>

#include <getopt.h>

#include "Kernel.h"

using std::string;

static char *g_stalkrc_path;
static char *g_log_path;
static char *g_stalkstate_path;
static bool g_colors;
static int g_out_spacing;

static void process_args(int *argc, char ***argv);
static void init(int *argc, char ***argv);
static void usage(FILE *str, const char *a0, int ec);


static void
process_args(int *argc, char ***argv)
{
	char *a0 = (*argv)[0];

	for(int ch; (ch = getopt(*argc, *argv, "f:s:l:ch")) != -1;) {
		switch (ch) {
		case 'h':
			usage(stdout, a0, EXIT_SUCCESS);
			break;
		case 'c':
			g_colors = true;
			break;
		case 's':
			g_out_spacing = (int)strtol(optarg, NULL, 10);
			break;
		case 'l':
			g_log_path = strdup(optarg);
			break;
		case 'f':
			g_stalkrc_path = strdup(optarg);
			break;
		case 'S':
			g_stalkstate_path = strdup(optarg);
			break;
		case '?':
		default:
			usage(stderr, a0, EXIT_FAILURE);
		}
	}

	*argc -= optind;
	*argv += optind;
}


static void
init(int *argc, char ***argv)
{
	process_args(argc, argv);
	if (!g_stalkrc_path || strlen(g_stalkrc_path) == 0) {
		char path[256];
		const char *home = getenv("HOME");
		if (!home) {
			warnx("no $HOME defined, using cwd");
			home = ".";
		}

		snprintf(path, sizeof path, "%s/.stalkrc", home);
		g_stalkrc_path = strdup(path);
	}

	if (!g_stalkstate_path || strlen(g_stalkstate_path) == 0) {
		char path[256];
		const char *home = getenv("HOME");
		if (!home) {
			warnx("no $HOME defined, using cwd");
			home = ".";
		}

		snprintf(path, sizeof path, "%s/.stalkstate", home);
		g_stalkstate_path = strdup(path);
	}
}


static void
usage(FILE *str, const char *a0, int ec)
{
	#define I(STR) fputs(STR "\n", str)
	I("=================================");
	I("== stalkware "PACKAGE_VERSION" - blah bleh ==");
	I("=================================");
	fprintf(str, "usage: %s [-h]\n", a0);
	I("");
	I("\t-h: Display brief usage statement and terminate");
	I("\t-f <path>: use this rcfile (default ~/.stalkrc)");
	I("\t-S <path>: use this statefile (default ~/.stalkstate)");
	I("\t-c: use bash-style color sequences on stdout");
	I("\t-l <path>: log events to this file (appending)");
	I("");
	I("(C) 2013, Timo Buhrmester (contact: #fstd @ irc.freenode.org)");
	#undef I
	exit(ec);
}


int
main(int argc, char **argv)
{
	init(&argc, &argv);

	Kernel *k = new Kernel;
	k->init(string(g_stalkrc_path), string(g_stalkstate_path),
			g_out_spacing, g_colors, string(g_log_path));
	bool ok = k->run();

	delete k;

	return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
