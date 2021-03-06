﻿/*
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

#ifndef BICOMC_DETAIL_COMPILER_GCC_ATOMIC_H__
#define BICOMC_DETAIL_COMPILER_GCC_ATOMIC_H__

#if !defined(__GNUC__)
#	error "compiler is not gcc"
#endif // !def __GNUC__

#include "../../stdint.h"
#include "../../type_traits.h"

namespace bcc
{
namespace detail
{
#if (__GNUC__ >= 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4
	inline void atomic_store(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		__atomic_store_n(pObject, desired, __ATOMIC_SEQ_CST);
	}

	inline bcc::intptr_t atomic_load(atomic_intptr_t const* pObject) BICOMC_NOEXCEPT
	{
		return __atomic_load_n(pObject, __ATOMIC_SEQ_CST);
	}

	inline bcc::intptr_t atomic_exchange(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		return __atomic_exchange_n(pObject, desired, __ATOMIC_SEQ_CST);
	}

	inline bool atomic_compare_exchange_strong(atomic_intptr_t* pObject, bcc::intptr_t* pExpected, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		return __atomic_compare_exchange_n(pObject, pExpected, desired, false, __ATOMIC_SEQ_CST, __ATOMIC_SEQ_CST);
	}

#else
	inline void atomic_store(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		__sync_lock_test_and_set(pObject, desired);
	}

	inline bcc::intptr_t atomic_load(atomic_intptr_t const* pObject) BICOMC_NOEXCEPT
	{
		return __sync_fetch_and_or(const_cast<atomic_intptr_t*>(pObject), 0);
	}

	inline bcc::intptr_t atomic_exchange(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		return __sync_lock_test_and_set(pObject, desired);
	}

	inline bool atomic_compare_exchange_strong(atomic_intptr_t* pObject, bcc::intptr_t* pExpected, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		bcc::intptr_t e = *pExpected;
		return e == (*pExpected = __sync_val_compare_and_swap(pObject, e, desired));
	}

#endif // (__GNUC__ >= 4 && __GNUC_MINOR__ >= 7) || __GNUC__ > 4

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_COMPILER_GCC_ATOMIC_H__
