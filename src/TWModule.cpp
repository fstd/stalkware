/* TWModule.cpp - stalkware(1) TW module implementation
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

#include "TWModule.h"

TWModule::TWModule()
: Module(),
  name_("N/A"),
  pstr_("TW")
{
}

TWModule::~TWModule()
{
}

bool
TWModule::init(string const& name, map<string, cfgent> const& cfg)
{
	name_ = name;
	return true;
}

string const&
TWModule::name() const
{
	return name_;
}

void
TWModule::check(vector<vector<string> > const& stalkees,
		vector<pair<string, string> > & result) const
{
	if (stalkees.size() == 0)
		return;
	
	set<string> inames;
	for(vector<vector<string> >::const_iterator it = stalkees.begin();
			it != stalkees.end(); it++)
		for(vector<string>::const_iterator itt = it->begin() + 1;
				itt != it->end(); itt++)
			inames.insert(*itt);
	
	vector<string> masters;

	//masters.push_back("master1.teeworlds.com");
	masters.push_back("master2.teeworlds.com");
	masters.push_back("master3.teeworlds.com");
	//masters.push_back("master4.teeworlds.com");

	tw::MasterComm m;
	m.SetMasters(masters);

	vector<string> res;
	m.GetList(res);

	tw::InfoComm i;
	i.SetServers(res);
	int suc = i.Refresh();

	for(map<string, tw::ServerInfo>::const_iterator
			it = i.GetAll().begin();
			it != i.GetAll().end(); it++) {
		if (!it->second.on_)
			continue;

		for(vector<tw::PlayerInfo>::const_iterator cit = it->second.clt_.begin();
				cit != it->second.clt_.end(); cit++) {
			if (!inames.count(cit->name_))
				continue;

			for(vector<vector<string> >::const_iterator
					sit = stalkees.begin();
					sit != stalkees.end(); sit++) {
				for(vector<string>::const_iterator itt =
						sit->begin() + 1;
						itt != sit->end(); itt++) {
					if (*itt != cit->name_)
						continue;
					
					result.push_back(pair<string, string>(
						(*sit)[0], cit->name_));
				}
			}
		}
	}
}
