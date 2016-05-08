#ifndef BICOMC_DETAIL_ERROR_H__
#define BICOMC_DETAIL_ERROR_H__

#include "config.h"

#include "../stdint.h"
#include "../object.h"

namespace bcc
{
namespace detail
{
	BICOMC_INTERFACE(ErrorDetail)
		BICOMC_DECL_METHOD_0_C(value, bcc::uint32_t());
		BICOMC_DECL_METHOD_0_C(category, bcc::uint32_t());
		BICOMC_DECL_METHOD_0_C(message, char const*());
	BICOMC_INTERFACE_END(ErrorDetail);
} // namespace detail
} // namespace bcc

BICOMC_SIGNATURE(bcc::detail::ErrorDetail);

#endif // !def BICOMC_DETAIL_ERROR_H__
