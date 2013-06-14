/* Kernel.cpp - stalkware(1) kernel implementation
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include <iostream>

#include "Kernel.h"

Kernel::Kernel()
: facmap_(facarr, facarr + (sizeof facarr / sizeof *facarr))
{
}

Kernel::~Kernel()
{
}

bool
Kernel::run()
{
	//Module *mod = facmap_["DummyModule"]();
	std::cerr << "i ran\n";
	return true;
}
