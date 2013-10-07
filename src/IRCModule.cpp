/* IRCModule.cpp - stalkware(1) IRC module implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <err.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cstdarg>

#include "IRCModule.h"

IRCModule::IRCModule()
: Module(),
  name_("N/A"),
  pstr_("IRC"),
  conto_(30000000ul),
  sendwait_(1),
  keepalive_(false),
  whoisto_(10)
{
}

IRCModule::~IRCModule()
{
	ircbas_dispose(irc_);
}

bool
IRCModule::init(string const& name, map<string, cfgent> const& cfg)
{
	name_ = name;

	if (!cfg.count("server")) {
		warnx("missing %s_server in rcfile", name.c_str());
		return false;
	}

	irc_ = ircbas_init();
	if (!irc_) {
		warnx("failed to create irc handle");
		return false;
	}

	ircbas_set_server(irc_, cfg.at("server").val.str_, (unsigned short)
			(cfg.count("port")?cfg.at("port").val.lng_:6667));

	char nick[10];
	for(size_t i = 0; i < sizeof nick; i++)
		nick[i] = 'a' + ((rand()>>3)%26);
	
	nick[sizeof nick - 1] = '\0';

	ircbas_set_nick(irc_, nick);

	if (cfg.count("pass"))
		ircbas_set_pass(irc_, cfg.at("pass").val.str_);
	ircbas_set_conflags(irc_, 8);

	if (cfg.count("conto"))
		conto_ = (unsigned long)cfg.at("conto").val.lng_;

	if (cfg.count("whoisto_"))
		whoisto_ = (int)cfg.at("whoisto").val.lng_;

	if (cfg.count("keepalive"))
		keepalive_ = (bool)cfg.at("keepalive").val.lng_;

	if (cfg.count("sendwait"))
		sendwait_ = (int)cfg.at("sendwait").val.lng_;

	return true;
}

string const&
IRCModule::name() const
{
	return name_;
}

void
IRCModule::check(vector<vector<string> > const& stalkees,
		vector<pair<string, string> > & result) const
{
	if (stalkees.size() == 0)
		return;
	
	if (!ircbas_online(irc_)) {
		if (!ircbas_connect(irc_, conto_)) {
			warnx("failed to logon to IRC");
			ircbas_reset(irc_);
			throw (std::exception());
		}
	}

	time_t nextsend = time(NULL);

	for(vector<vector<string> >::const_iterator sit = stalkees.begin();
			sit != stalkees.end(); sit++) {
		string extname = (*sit)[0];
		for(vector<string>::const_iterator nit = sit->begin() + 1;
				nit != sit->end(); nit++) {

			string intname = *nit;

			while(time(NULL) < nextsend)
				sleep(1);

			nextsend = time(NULL) + sendwait_;

			if (find_user(intname, whoisto_)) {
				result.push_back(pair<string, string>(
						extname, intname));
				break;
			}
		}
	}

	if (!keepalive_)
		ircbas_reset(irc_);
}

bool
IRCModule::find_user(string const& intname, int to_s) const
{
	iprintf("WHOIS %s\r\n", intname.c_str());

	bool found = true;
	bool sure = false;

	time_t tend = time(NULL) + to_s;
	do {
		char *tok[16];
		int r = ircbas_read(irc_, tok, 16, 1000);
		if (r == 0)
			continue;

		if (r < 0) {
			warnx("failed to read from server");
			ircbas_reset(irc_);
			throw (std::exception());
		}

		if (strcmp(tok[1], "PING") == 0) {
			iprintf("PONG :%s", tok[2]);
		} else if (strcmp(tok[1], "401") == 0) {
			found = false;
		} else if (strcmp(tok[1], "402") == 0) {
			found = false;
		} else if (strcmp(tok[1], "318") == 0) {
			sure = true;
			break;
		}
	} while(time(NULL) <= tend); //just < could suck.

	return sure ? found : false;
}

void
IRCModule::iprintf(const char *fmt, ...) const
{
	va_list l;
	va_start(l, fmt);
	char buf[1024];
	vsnprintf(buf, sizeof buf, fmt, l);
	va_end(l);

	if (!ircbas_write(irc_, buf)) {
		warnx("failed to write to server");
		ircbas_reset(irc_);
		throw (std::exception());
	}
}
