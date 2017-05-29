
#ifndef BICOMC_DETAIL_COMPILER_NAIVE_ATOMIC_H__
#define BICOMC_DETAIL_COMPILER_NAIVE_ATOMIC_H__

#include "../../stdint.h"

namespace bcc
{
namespace detail
{
	inline void atomic_store(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		atomic_intptr_t volatile* p = pObject;
		*p = desired;
	}

	inline bcc::intptr_t atomic_load(atomic_intptr_t const* pObject) BICOMC_NOEXCEPT
	{
		atomic_intptr_t const volatile* p = pObject;
		return *p;
	}

	inline bcc::intptr_t atomic_exchange(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		atomic_intptr_t volatile* p = pObject;
		bcc::intptr_t old;
		do
		{
			old = *p;
			*p = desired;
		} while (*p != desired);
		return old;
	}

	inline bool atomic_compare_exchange_strong(atomic_intptr_t* pObject, bcc::intptr_t* pExpected, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		atomic_intptr_t volatile* p = pObject;
		bcc::intptr_t old, e = *pExpected;
		do
		{
			old = *p;
			if (old != e) break;
			*p = desired;
		} while (*p != desired);
		*pExpected = old;
		return old == e;
	}

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_COMPILER_NAIVE_ATOMIC_H__
