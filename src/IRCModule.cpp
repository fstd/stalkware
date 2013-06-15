/* IRCModule.cpp - stalkware(1) IRC module implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "IRCModule.h"

IRCModule::IRCModule()
: Module(),
  name_("N/A"),
  pstr_("IRC")
{
}

IRCModule::~IRCModule()
{
}

bool
IRCModule::init(string const& name, map<string, cfgent> const& cfg)
{
	name_ = name;
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
	for(vector<vector<string> >::const_iterator sit = stalkees.begin();
			sit != stalkees.end(); sit++) {
		string extname = (*sit)[0];
		for(vector<string>::const_iterator nit = sit->begin() + 1;
				nit != sit->end(); nit++) {

			string intname = *nit;
			if (find_user(intname)) {
				result.push_back(pair<string, string>(
						extname, intname));
				break;
			}
		}
	}
}

bool
IRCModule::find_user(string const& intname) const
{
	return false;
}
