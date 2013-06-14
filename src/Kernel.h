/* Kernel.h - stalkware(1) kernel interface (for launcher)
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_KERNEL_H
#define STALKWARE_KERNEL_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "Module.h"

class Kernel {
public:
	bool run();
};

#endif
