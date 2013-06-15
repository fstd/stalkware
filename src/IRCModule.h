/* IRCModule.h - stalkware(1) IRC module interface
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_IRCMODULE_H
#define STALKWARE_IRCMODULE_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "Module.h"

class IRCModule : public Module {
private:
	string name_;
	string pstr_;

	bool find_user(string const& intname) const;

public:
	IRCModule();
	virtual ~IRCModule();

	virtual void init(string const& name,
			map<string, cfgent> const& cfg);

	virtual string const& name() const;
	virtual string const& pname() const { return pstr_; };

	virtual void check(vector<vector<string> > const& stalkees,
			vector<pair<string, string> > & result) const;
};

#endif
