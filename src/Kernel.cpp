/* Kernel.cpp - stalkware(1) kernel implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <deque>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cerrno>

#include <err.h>

#include "Kernel.h"

using std::deque;

Kernel::Kernel()
: facmap_(facarr, facarr + (sizeof facarr / sizeof *facarr)),
  modmap_(),
  cfgmap_(),
  stmap_(),
  kerncfg_(),
  buddies_(),
  spc_(0)
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
	char *spc = NULL;

	for(map<string, Module*>::const_iterator it = modmap_.begin();
			it != modmap_.end(); it++) {
		Module *mod = it->second;
		map<string, cfgent> *cfg = cfgmap_[mod];
		mod->init(it->first, *cfg);
	}

	display(time(NULL));

	for(;;) {
		time_t tnext = time(NULL) + kerncfg_["pollint"].val.lng_;
		bool changed = false;
		for(map<string, Module*>::const_iterator
				it = modmap_.begin(); it != modmap_.end();
				it++) {
			Module *mod = it->second;
			vector<pair<string, string> > res;
			try {
				it->second->check(stmap_[mod], res);
				for(vector<pair<string, string> >::
						const_iterator itt =
						res.begin(); itt !=
						res.end(); itt++) {
					changed = true;
					buddies_[itt->first].tlast = time(NULL);
					buddies_[itt->first].plast = mod->pname();
					buddies_[itt->first].mlast = mod->name();
					buddies_[itt->first].ilast = itt->second;
				}
			} catch(std::exception e) {
				warnx("caught exception");
			}
		}

		if (changed) {
			save_stalkstate(statepath_);
		}


		if (spc_) {
			if (!spc) {
				spc = new char[spc_];
				memset(spc, '\n', spc_-1);
				spc[spc_-1] = '\0';
			} 

			puts(spc);
		}

		time_t now = time(NULL);

		display(now);

		if (now < tnext)
			sleep(tnext - now);
	}

	return true;
}

void
Kernel::display(time_t now)
{
	deque<string> unordered;
	deque<string> ordered;

	for(map<string, buddy>::const_iterator it = buddies_.begin();
			it != buddies_.end(); it++)
		unordered.push_back(it->first);

	while(unordered.size() > 0) {
		time_t tmax = 0;
		string maxkey = "";
		int maxpos = 0;
		int c = 0;
		for(deque<string>::const_iterator
				it = unordered.begin();
				it != unordered.end(); it++) {
			if (maxkey=="" || buddies_[*it].tlast > tmax) {
				tmax = buddies_[*it].tlast;
				maxkey = *it;
				maxpos = c;
			}
			c++;
		}

		ordered.push_back(maxkey);
		unordered.erase(unordered.begin() + maxpos);
	}
	for(deque<string>::const_iterator it = ordered.begin();
			it != ordered.end(); it++) {
		buddy const& b = buddies_[*it];
		if (!b.tlast) {
			printf("%s (never seen)\n", it->c_str());
		} else {
			int sago = (int)(now-b.tlast);
			char schar = 's';
			if (sago > 60*60*24*365) {
				schar = 'y';
				sago /= 60*60*24*365;
			} else if (sago > 60*60*24) {
				schar = 'd';
				sago /= 60*60*24;
			} else if (sago > 60*60) {
				schar = 'h';
				sago /= 60*60;
			} else if (sago > 60) {
				schar = 'm';
				sago /= 60;
			}

			printf("%s (%d%c; %s@%s/%s)\n",
					it->c_str(), sago, schar,
					b.ilast.c_str(), b.plast.c_str(),
					b.mlast.c_str());
		}
	}
}

void
Kernel::init(string const& stalkrc, string const& stalkstate, int spacing)
{
	spc_ = spacing;
	statepath_ = stalkstate;
	process_stalkrc(stalkrc);
	load_stalkstate(statepath_);
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
Kernel::load_stalkstate(string const& path)
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

		string name(strtok(dup, "\t"));
		if (!buddies_.count(name)) {
			warnx("unknown buddy '%s' in state file",
					name.c_str());
			free(dup);
			continue;
		}
			

		buddies_[name].tlast = (time_t)strtol(strtok(NULL, "\t"),
				NULL, 10);
		buddies_[name].mlast = string(strtok(NULL, "\t"));
		buddies_[name].plast = string(strtok(NULL, "\t"));
		buddies_[name].ilast = string(strtok(NULL, "\n"));

		free(dup);
	}

	fclose(fp);
	free(line);
}

void
Kernel::save_stalkstate(string const& path)
{
	errno = 0;
	FILE *fp = fopen(path.c_str(), "w");
	if (!fp) {
		warn("couldn't fopen() '%s'", path.c_str());
		return;
	}
	
	for(map<string, buddy>::const_iterator it = buddies_.begin();
			it != buddies_.end(); it++) {
		if (it->second.tlast == 0)
			continue;
		char buf[1024];
		snprintf(buf, 1024, "%s\t%lu\t%s\t%s\t%s\n",
				it->first.c_str(),
				(unsigned long)it->second.tlast,
				it->second.mlast.c_str(),
				it->second.plast.c_str(),
				it->second.ilast.c_str());
		fputs(buf, fp);
	}

	fclose(fp);
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
		load_mod(modname, modclass);

		free(modclass);
		free(modname);

	} else if (strcmp(tok, "stalk") == 0) {
		tok = strtok_r(NULL, "\t ", &ctx);
		char *modname = strdup(tok);
		tok = strtok_r(NULL, "\0", &ctx);
		char *remainder = strdup(tok);
		vector<char*> namevec;

		char *start = remainder;
		char *tokstart, *tokend;
		while(nextqtok(start, &tokstart, &tokend)) {
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
	}

	string key(user[0]);
	buddies_[key].tlast = 0;
	buddies_[key].plast = "N/A";
	buddies_[key].mlast = "N/A";
	buddies_[key].ilast = "N/A";
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

