/* Kernel.h - stalkware(1) kernel interface (for launcher)
 * stalkware - blah bleh - (C) 2013, Timo Buhrmester
 * See README for contact-, COPYING for license information.  */

#ifndef STALKWARE_KERNEL_H
#define STALKWARE_KERNEL_H 1

#if HAVE_CONFIG_H
# include <config.h>
#endif

#include "Module.h"

typedef Module* (*modfac_fp)();
template<typename T> Module* spawn() { return new T; }

struct modreg_s {
	const char *modnam_;
	modfac_fp modfac_;
	operator map<string, modfac_fp>::value_type() const {
		return map<string, modfac_fp>::value_type(modnam_, modfac_);
	}
};

#define REG(MOD) {#MOD, &spawn<MOD>}
// for each new module, add the include here:
#include "DummyModule.h"

static struct modreg_s facarr[] = {
// and register here:
	REG(DummyModule)
};

#undef REG

class Kernel {
public:
	Kernel();
	~Kernel();

	bool run();

private:
	map<string, modfac_fp> facmap_;
};

#endif
