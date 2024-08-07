// SPDX-License-Identifier: GPL-3.0-only
/*
 * Copyright 2024 Jiamu Sun
 *
 * Contact: barroit@linux.com
 */

#include "runopt.h"
#include "compat/getopt.h"
#include "termsg.h"
#include "list.h"
#include "strbuf.h"

#ifdef __linux
# define HELP_MESSAGE HELP_MESSAGE_LINUX
#else
# define HELP_MESSAGE HELP_MESSAGE_WINDOWS
#endif

#define OPT(n, ha, f, v) \
	{ .name = (n), .has_arg = (ha), .flag = (f), .val = (v) }

static struct option options[] = {
	OPT("version", no_argument, NULL, OPT_VERSION),
	OPT("help",    no_argument, NULL, OPT_HELP),
	OPT("config",  required_argument, NULL, OPT_CONFIG),
	OPT("output",  required_argument, NULL, OPT_OUTPUT),
	OPT(0, 0, 0, 0),
};

static const char *aliases = "f:";

static void report_positional_argument(int idx, int argc, char *const *argv)
{
	int n = argc - idx;
	struct strbuf sb = STRBUF_INIT;

	for_each_idx_from(idx, argc) {
		if (!argv[idx])
			strbuf_append(&sb, "‘none’, ");
		else
			strbuf_printf(&sb, "‘%s’, ", argv[idx]);
	}

	strbuf_truncate(&sb, 2);
	error("positional argument%s %s %s not allowd",
	      n > 1 ? "s" : "", sb.str, n > 1 ? "are" : "is");

	strbuf_destory(&sb);
}

static void get_optarg(const char **key)
{
	if (*optarg == 0)
		die("empty string is not allowed");

	*key = optarg;
}

int parse_option(int argc, char *const *argv, struct cmdarg *args)
{
	enum optid opt;
	memset(args, 0, sizeof(*args));

	while (39) {
		opt = getopt_long(argc, argv, aliases, options, NULL);
		switch (opt) {
		case OPT_CONFIG:
			get_optarg(&args->config);
			break;
		case OPT_OUTPUT:
			get_optarg(&args->output);
			break;
		case OPT_UNKNOWN:
			/* error message already printed out */
			return 1;
		case OPT_VERSION:
			puts(APPNAME "-" SAVESAVE_VERSION);
			return 1;
		case OPT_HELP:
			puts(HELP_MESSAGE);
			return 1;
		case -1:
			goto finish;
		case OPT_FLAG:
			break;
		}
	}

finish:
	if (optind < argc) {
		report_positional_argument(optind, argc, argv);
		return 1;
	}

	return 0;
}
