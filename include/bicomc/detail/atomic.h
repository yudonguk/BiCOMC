/*
 * Copyright 2016 Kangwon National University and Contributors
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

#ifndef BICOMC_DETAIL_ATOMIC_H__
#define BICOMC_DETAIL_ATOMIC_H__

#include "config.h"

#include "../stdint.h"

namespace bcc
{
namespace detail
{
	typedef bcc::intptr_t atomic_intptr_t;

	void atomic_store(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT;
	bcc::intptr_t atomic_load(atomic_intptr_t const* pObject) BICOMC_NOEXCEPT;
	bcc::intptr_t atomic_exchange(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT;
	bool atomic_compare_exchange_strong(atomic_intptr_t* pObject, bcc::intptr_t* pExpected, bcc::intptr_t desired) BICOMC_NOEXCEPT;
} // namespace detail
} // namespace bcc

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "compiler/msvc_atomic.h"

#elif defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "compiler/gcc_atomic.h"

#elif defined(__INTEL_COMPILER)
#	include "compiler/intel_atomic.h"

#elif defined(__clang__)
#	include "compiler/clang_atomic.h"

#else
#	if !defined(BICOMC_ENABLE_NAIVE_ATOMIC)
#		error "Please write atomic operations consisted of compiler intrinsics or define 'BICOMC_ENABLE_NAIVE_ATOMIC' macro to dismiss this error."
#	endif

#	include "compiler/naive_atomic.h"

#endif

#endif // !def BICOMC_DETAIL_ATOMIC_H__
