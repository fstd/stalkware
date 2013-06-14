/* stalkware.cpp - stalkware(1) launcher
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdio>
#include <cstdlib>

#include <getopt.h>


static void process_args(int *argc, char ***argv);
static void init(int *argc, char ***argv);
static void usage(std::FILE *str, const char *a0, int ec);


static void
process_args(int *argc, char ***argv)
{
	char *a0 = (*argv)[0];

	for(int ch; (ch = getopt(*argc, *argv, "h")) != -1;) {
		switch (ch) {
		case 'h':
			usage(stdout, a0, EXIT_SUCCESS);
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
}


static void
usage(std::FILE *str, const char *a0, int ec)
{
	#define I(STR) std::fputs(STR "\n", str)
	I("===========================");
	I("== stalkware - blah bleh ==");
	I("===========================");
	std::fprintf(str, "usage: %s [-h]\n", a0);
	I("");
	I("\t-h: Display brief usage statement and terminate");
	I("");
	I("(C) 2013, Timo Buhrmester (contact: #fstd @ irc.freenode.org)");
	#undef I
	std::exit(ec);
}


int
main(int argc, char **argv)
{
	init(&argc, &argv);

	std::exit(EXIT_SUCCESS);
}
