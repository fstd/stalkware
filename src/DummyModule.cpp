/* DummyModule.cpp - stalkware(1) dummy module implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <iostream>

#include <ctime>
#include <cstdlib>

#include "DummyModule.h"

#define DEF_MINROLL 4

void
DummyModule::init(string const& name, map<string, union cfgent> const& cfg)
{
	std::srand(std::time(NULL));
	name_ = name;
	minroll_ = cfg.count("minroll")
			? cfg.at("minroll").int_ : DEF_MINROLL;
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
		std::cerr << "extname: " << extname << std::endl;
		for(vector<string>::const_iterator nit = sit->begin() + 1;
				nit != sit->end(); nit++) {

			string intname = *nit;
			std::cerr << "\tintname: " << intname << std::endl;
			if (find_user(intname)) {
				std::cerr << "\t\tonline!" << std::endl;
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
	return (std::rand()>>3) % 6 >= minroll_-1;
}
