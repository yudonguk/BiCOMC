/*
 * Copyright 2015 Kangwon National University and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

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
