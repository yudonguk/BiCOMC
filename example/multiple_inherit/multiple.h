#ifndef MULTIPLE_H__
#define MULTIPLE_H__

#include <bicomc/object.h>

BICOMC_INTERFACE(Simple)
	BICOMC_DECL_METHOD_0_C(hello, void());
	BICOMC_DECL_METHOD_0_C(number, bcc::int32_t());
	BICOMC_DECL_METHOD_1(number, bcc::int32_t(bcc::int32_t));
BICOMC_INTERFACE_END(Simple);

BICOMC_INTERFACE(OverlapHello)
	BICOMC_DECL_METHOD_0_C(hello, void());
BICOMC_INTERFACE_END(OverlapHello);

#endif // def !MULTIPLE_H__
