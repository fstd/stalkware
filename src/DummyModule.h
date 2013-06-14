/* DummyModule.h - stalkware(1) dummy module interface
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_DUMMYMODULE_H
#define STALKWARE_DUMMYMODULE_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "Module.h"

class DummyModule : public Module {
private:
	std::string name_;
	int minroll_;

	bool find_user(string const& intname) const;

public:
	virtual void init(string const& name,
			map<string, union cfgent> const& cfg);

	virtual string const& name() const;

	virtual void check(vector<vector<string> > const& stalkees,
			vector<pair<string, string> > & result) const;
};

#endif
