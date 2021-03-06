﻿#ifndef SIMPLE_H__
#define SIMPLE_H__

#include <bicomc/object.h>

BICOMC_INTERFACE(Simple)
	BICOMC_DECL_METHOD_0_C(hello, void());
	BICOMC_DECL_METHOD_0_C(number, bcc::int32_t());
	BICOMC_DECL_METHOD_1(number, bcc::int32_t(bcc::int32_t));
BICOMC_INTERFACE_END(Simple);

#endif // !def SIMPLE_H__
