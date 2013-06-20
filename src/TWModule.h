/* TWModule.h - stalkware(1) TW module interface
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_TWMODULE_H
#define STALKWARE_TWMODULE_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <libtw/master.h>
#include <libtw/info.h>

#include "Module.h"

class TWModule : public Module {
private:
	string name_;
	string pstr_;

public:
	TWModule();
	virtual ~TWModule();

	virtual bool init(string const& name,
			map<string, cfgent> const& cfg);

	virtual string const& name() const;
	virtual string const& pname() const { return pstr_; };

	virtual void check(vector<vector<string> > const& stalkees,
			vector<pair<string, string> > & result) const;
};

#endif
