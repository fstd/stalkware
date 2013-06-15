/* DummyModule.cpp - stalkware(1) dummy module implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <cstdlib>
#include <cstdio>
#include <ctime>

#include "DummyModule.h"

#define DEF_MINROLL 4

DummyModule::DummyModule()
: Module(),
  name_("N/A"),
  minroll_(DEF_MINROLL),
  pstr_("DummyPlatform")
{
}

DummyModule::~DummyModule()
{
}

bool
DummyModule::init(string const& name, map<string, cfgent> const& cfg)
{
	srand(time(NULL));
	name_ = name;
	if (cfg.count("minroll"))
		minroll_ = (int)cfg.at("minroll").val.lng_;
	return true;
}

string const&
DummyModule::name() const
{
	return name_;
}

void
DummyModule::check(vector<vector<string> > const& stalkees,
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
DummyModule::find_user(string const& intname) const
{
	return (rand()>>3) % 6 >= minroll_-1;
}
