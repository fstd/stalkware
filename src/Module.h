/* Module.h - stalkware(1) module ABC
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_MODULE_H
#define STALKWARE_MODULE_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <vector>
#include <map>
#include <utility> //pair
#include <string>

using std::vector;
using std::string;
using std::pair;
using std::map;

class Module {
public:
	/* initialize the module with its name and a configuration map
	 * you may assume reasonable defaults for missing cfg entries,
	 * however it is also fine to signal initialization failure
	 * by returning false. if everything went good, return true.
	 * store the name for later retrieval by name() */
	virtual bool init(string const& name,
			map<string, struct cfgent> const& cfg) = 0;

	/* return the name which was given to init() */
	virtual string const& name() const = 0;

	/* return the name of the platform (e.g. "IRC") this module
	 * is dealing with. */
	virtual string const& pname() const = 0;

	/* check your realm for online stalkees.
	 * ,,stalkees'' is a vector of users to stalk. Each stalkee has one
	 * 'external' name, and one or more 'internal' names.
	 * The 'external' name is for displaying purposes only and as such
	 * freely choosable; the 'internal' names are the realm-specific
	 * tokens which are actually looked for when check()ing.
	 * By definition, the 'external' name is the first element in a
	 * vector<string> representing a stalkee, the 'internal' names
	 * are the remaining elements in that vector.
	 * The outer vector simply accumulates multiple such stalkee
	 * representations.
	 * Example (2 stalkees):
	 * { {"teh learath guy", "Learath2", "lea2"},
	 *   {"yuri", "eeeee", "eeeee_", "eeeeee", "clippy"} }
	 *
	 * ,,result'' is the structure in which to store the results, i.e.
	 * the successfully found stalkees.
	 * A pair<string, string> represents a found user; the first
	 * element must be their external name, the second element is to
	 * be the internal name under which the client was actually found.
	 *
	 * if something goes wrong, throw std::exception (for now).
	 */
	virtual void check(vector<vector<string> > const& stalkees,
			vector<pair<string, string> > & result) const = 0;

	/* a virtual dtor since we're an ABC */
	virtual ~Module() {}
};

struct cfgent {
	enum {LNG,DBL,STR} typ;
	union {
		long lng_;
		double dbl_;
		char* str_;
	} val;
};

#endif
