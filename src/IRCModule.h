/* IRCModule.h - stalkware(1) IRC module interface
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_IRCMODULE_H
#define STALKWARE_IRCMODULE_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

extern "C" {
#include <libsrsirc/irc_basic.h>
#include <libsrsirc/irc_util.h>
}

#include "Module.h"

class IRCModule : public Module {
private:
	string name_;
	string pstr_;
	ibhnd_t irc_;
	unsigned long conto_; //microseconds
	int sendwait_; //seconds
	bool keepalive_;

	bool find_user(string const& intname) const;
	void iprintf(const char *fmt, ...) const;

public:
	IRCModule();
	virtual ~IRCModule();

	virtual bool init(string const& name,
			map<string, cfgent> const& cfg);

	virtual string const& name() const;
	virtual string const& pname() const { return pstr_; };

	virtual void check(vector<vector<string> > const& stalkees,
			vector<pair<string, string> > & result) const;
};

#endif
