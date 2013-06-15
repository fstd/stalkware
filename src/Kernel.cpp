/* Kernel.cpp - stalkware(1) kernel implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <err.h>

#include "Kernel.h"

Kernel::Kernel()
: facmap_(facarr, facarr + (sizeof facarr / sizeof *facarr)),
  modmap_(),
  cfgmap_(),
  stmap_(),
  kerncfg_()
{
}

Kernel::~Kernel()
{
}

void
Kernel::dump()
{
	/*for(map<Module*, map<string, cfgent>*>::const_iterator
			it = cfgmap_.begin(); it != cfgmap_.end(); it++) {
	}*/

	for(map<Module*, vector<vector<string> > >::const_iterator
			it = stmap_.begin(); it != stmap_.end(); it++) {
		fprintf(stderr, "stmap[%s] dump:\n",
				it->first->name().c_str());

		const vector<vector<string> > &vec = it->second;

		for(vector<vector<string> >::const_iterator
				st = vec.begin(); st != vec.end(); st++) {
			fprintf(stderr, "\t{\n");

			const vector<string> &nvec = *st;

			for(vector<string>::const_iterator
					nt = nvec.begin();
					nt != nvec.end(); nt++) {
				fprintf(stderr, "\t\t'%s'\n", nt->c_str());
			}

			fprintf(stderr, "\t}\n");

		}

	}


}

bool
Kernel::run()
{
	for(map<string, Module*>::const_iterator it = modmap_.begin();
			it != modmap_.end(); it++) {
		Module *mod = it->second;
		map<string, cfgent> *cfg = cfgmap_[mod];
		fprintf(stderr, "initializing module '%s'\n",
				it->first.c_str());
		mod->init(it->first, *cfg);
	}

	return true;
}

void
Kernel::init(string const& stalkrc)
{
	process_stalkrc(stalkrc);
}

void
Kernel::process_stalkrc(string const& path)
{
	char *line = NULL;
	size_t len = 0;
	ssize_t read;

	errno = 0;
	FILE *fp = fopen(path.c_str(), "r");
	if (!fp) {
		warn("couldn't fopen() '%s'", path.c_str());
		return;
	}

	while ((read = getline(&line, &len, fp)) != -1) {
		char *dup = strdup(line);
		char *ptr = strchr(dup, '#');
		if (ptr)
			*ptr = 0;

		ptr = dup + strlen(dup) - 1;
		while(ptr >= dup && isspace(*ptr))
			*ptr-- = '\0';

		ptr = dup;
		while(isspace(*ptr))
			ptr++;

		if (strlen(ptr) == 0)
			continue;

		process_cfgline(ptr);

		free(dup);
	}

	fclose(fp);
	free(line);
}

void
Kernel::process_cfgline(const char *line)
{
	char *ctx, *ctx2; //context pointers for strtok_r
	char *dup = strdup(line);

	char *tok = strtok_r(dup, "\t ", &ctx);
	if (strcmp(tok, "load") == 0) {
		tok = strtok_r(NULL, "\t ", &ctx);
		char *modclass = strdup(tok);
		tok = strtok_r(NULL, "\t ", &ctx);
		char *modname = strdup(tok);
		if (strtok_r(NULL, "\t ", &ctx))
			warnx("too many tokens (some ignored): '%s'", line);
		fprintf(stderr, "load: modclass '%s', modname '%s'\n",
				modclass, modname);

		load_mod(modname, modclass);

		free(modclass);
		free(modname);

	} else if (strcmp(tok, "stalk") == 0) {
		tok = strtok_r(NULL, "\t ", &ctx);
		char *modname = strdup(tok);
		tok = strtok_r(NULL, "\0", &ctx);
		char *remainder = strdup(tok);
		vector<char*> namevec;

		fprintf(stderr, "stalk: modname '%s', remainder '%s'\n",
				modname, remainder);

		char *start = remainder;
		char *tokstart, *tokend;
		while(nextqtok(start, &tokstart, &tokend)) {
			fprintf(stderr, "tok: '%*.*s'\n",
					(int)(tokend-tokstart),
					(int)(tokend-tokstart), tokstart);

			namevec.push_back(strndup(tokstart, (size_t)(tokend-tokstart)));

			start = tokend;
			if (*tokend)
				start++;
		}

		add_stalkee(modname, namevec);

		for(vector<char*>::const_iterator it = namevec.begin();
				it != namevec.end(); it++)
			free(*it);

		free(modname);
		free(remainder);

	} else {
		char *cmddup = strdup(tok);
		char *mod = NULL, *itok;
		if (strchr(cmddup, '_')) {
			itok = strtok_r(cmddup, "_", &ctx2);
			mod = strdup(itok);
			itok = strtok_r(NULL, "\t ", &ctx2);
		} else {
			itok = strtok_r(cmddup, "\t ", &ctx2);
		}

		char *sett = strdup(itok);
		itok = strtok_r(NULL, "\0", &ctx);
		while(isspace(*itok))
			itok++;

		char *val = strdup(itok);
		fprintf(stderr, "set: mod '%s', sett '%s', val '%s'\n",
				mod, sett, val);

		add_cfg(mod, sett, val);

		free(val);
		free(sett);
		free(mod);
		free(cmddup);
	}

	free(dup);
}

bool
Kernel::nextqtok(char *data, char **tokstart, char **tokend)
{
	char *start = data;

	while(isspace(*start))
		start++;

	if(!*start)
		return false;

	bool inq = false;
	bool quoted = false;
	char *end = start;
	while(*end) {
		if (*end == '"') {
			if (!(inq = !inq)) //leaving quotes
				break;
			else
				quoted = true;
		} else if (isspace(*end) && !inq)
			break;

		end++;
	}

	if (quoted)
		start++;

	*tokstart = start;
	*tokend = end;

	return true;
}

void
Kernel::add_stalkee(const char *mname, vector<char*> const& user)
{
	if (!modmap_.count(std::string(mname))) {
		warnx("unknown module: '%s'", mname);
		return;
	}

	Module *mod = modmap_[mname];
	vector<vector<string> > &stvec = stmap_[mod];

	stvec.push_back(vector<string>());

	for(vector<char*>::const_iterator it = user.begin();
			it != user.end(); it++) {
		stvec.back().push_back(string(*it));
		fprintf(stderr, "pushed '%s'\n", string(*it).c_str());
	}
}

void
Kernel::add_cfg(const char *mname, const char *sett, const char *val)
{
	map<string, cfgent> *cfg;
	if (mname) {
		if (!modmap_.count(std::string(mname))) {
			warnx("unknown module: '%s'", mname);
			return;
		}

		Module *mod = modmap_[mname];
		cfg = cfgmap_[mod];
	} else
		cfg = &kerncfg_;


	if ((*val >= '0' && *val <= '9') || *val == '-' || *val == '.') {
		if (strchr(val, '.')||strchr(val, 'e')||strchr(val, 'E')) {
			//treat as double
			(*cfg)[sett].typ = cfgent::DBL;
			(*cfg)[sett].val.dbl_ = strtod(val, NULL);
		} else {
			//treat as long
			(*cfg)[sett].typ = cfgent::LNG;
			(*cfg)[sett].val.lng_ = strtol(val, NULL, 10);
		}
	} else {
		//treat as string
		(*cfg)[sett].typ = cfgent::STR;
		if (*val == '"') {
			bool endq = *(val+strlen(val)-1) == '"';
			(*cfg)[sett].val.str_ = strndup(val + 1,
					strlen(val) - endq?2:1);
		} else
			(*cfg)[sett].val.str_ = strdup(val);
	}
}

void
Kernel::load_mod(const char *mname, const char *mclass)
{
	if (!facmap_.count(std::string(mclass))) {
		warnx("no such class: '%s' (not registered in Kernel.h?)",
				mclass);
		return;
	}

	if (modmap_.count(std::string(mname))) {
		warnx("name already in use: '%s'", mname);
		return;
	}

	Module *mod = facmap_[mclass]();
	if (!mod) {
		warnx("failed to instantiate '%s'", mclass);
		return;
	}

	modmap_[mname] = mod;
	cfgmap_[mod] = new map<string, cfgent>;
	stmap_[mod] = vector<vector<string> >();
}

