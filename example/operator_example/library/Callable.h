#pragma once

#include <bicomc/object.h>

BICOMC_INTERFACE(Callable)
	BICOMC_DECL_OPERATOR_ASSIGN(void(bcc::Object* arg));
	BICOMC_DECL_OPERATOR_DEREFERENCE_C(bcc::Object*());

	BICOMC_DECL_OPERATOR_CALL_0(bcc::Object*());
	BICOMC_DECL_OPERATOR_CALL_1(bcc::Object*(bcc::Object* arg));

	BICOMC_DECL_OPERATOR_EQUAL_C(bcc::uint8_t(Callable const& rhs));
BICOMC_INTERFACE_END(Callable);
