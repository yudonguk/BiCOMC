#ifndef BICOMC_DETAIL_COMPILER_MSVC_ATOMIC_H__
#define BICOMC_DETAIL_COMPILER_MSVC_ATOMIC_H__

#if !defined(_MSC_VER)
#	error "compiler is not msvc"
#endif // !def _MSC_VER

#include "../../stdint.h"
#include "../../type_traits.h"

#include <intrin.h>

namespace bcc
{
namespace detail
{
	struct MsvcAtomic
	{
		template<typename Atomic>
		static bcc::intptr_t exchangeImpl(Atomic& object, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 1>::type*) BICOMC_NOEXCEPT
		{
			char* p = static_cast<char*>(&object);
			return ::_InterlockedExchange8(p, desired);
		}

		template<typename Atomic>
		static bcc::intptr_t exchangeImpl(Atomic& object, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 2>::type*) BICOMC_NOEXCEPT
		{
			short* p = static_cast<short*>(&object);
			return ::_InterlockedExchange16(p, desired);
		}

		template<typename Atomic>
		static bcc::intptr_t exchangeImpl(Atomic& object, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 4>::type*) BICOMC_NOEXCEPT
		{
			long* p = reinterpret_cast<long*>(&object);
			return ::_InterlockedExchange(p, desired);
		}

		template<typename Atomic>
		static bcc::intptr_t exchangeImpl(Atomic& object, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 8>::type*) BICOMC_NOEXCEPT
		{
			__int64* p = static_cast<__int64*>(&object);
			return ::_InterlockedExchange64(p, desired);
		}

		template<typename Atomic>
		static bcc::intptr_t exchangeImpl(Atomic& object, bcc::intptr_t desired, ...) BICOMC_NOEXCEPT
		{
			static_assert(false, "Can't support atomic exchange.");
		}

		template<typename Atomic>
		static bcc::intptr_t loadImpl(Atomic& object, typename bcc::enable_if<sizeof(Atomic) == 1>::type*) BICOMC_NOEXCEPT
		{
			char* p = static_cast<char*>(&object);
			return ::_InterlockedOr8(p, 0);
		}

		template<typename Atomic>
		static bcc::intptr_t loadImpl(Atomic& object, typename bcc::enable_if<sizeof(Atomic) == 2>::type*) BICOMC_NOEXCEPT
		{
			short* p = static_cast<short*>(&object);
			return ::_InterlockedOr16(p, 0);
		}

		template<typename Atomic>
		static bcc::intptr_t loadImpl(Atomic& object, typename bcc::enable_if<sizeof(Atomic) == 4>::type*) BICOMC_NOEXCEPT
		{
			long* p = reinterpret_cast<long*>(&object);
			return ::_InterlockedOr(p, 0);
		}

		template<typename Atomic>
		static bcc::intptr_t loadImpl(Atomic& object, typename bcc::enable_if<sizeof(Atomic) == 8>::type*) BICOMC_NOEXCEPT
		{
			__int64* p = static_cast<__int64*>(&object);
			return ::_InterlockedOr64(p, 0);
		}

		template<typename Atomic>
		static bcc::intptr_t loadImpl(Atomic& object, ...) BICOMC_NOEXCEPT
		{
			static_assert(false, "can't support atomic load.");
		}

		template<typename Atomic>
		static bool compareExchangeStrongImpl(Atomic& object, bcc::intptr_t& expected, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 1>::type*) BICOMC_NOEXCEPT
		{
			char* p = static_cast<char*>(&object);
			bcc::intptr_t e = expected;
			return e == (expected = ::_InterlockedCompareExchange8(p, desired, e));
		}

		template<typename Atomic>
		static bool compareExchangeStrongImpl(Atomic& object, bcc::intptr_t& expected, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 2>::type*) BICOMC_NOEXCEPT
		{
			short* p = static_cast<short*>(&object);
			bcc::intptr_t e = expected;
			return e == (expected = ::_InterlockedCompareExchange16(p, desired, e));
		}

		template<typename Atomic>
		static bool compareExchangeStrongImpl(Atomic& object, bcc::intptr_t& expected, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 4>::type*) BICOMC_NOEXCEPT
		{
			long* p = reinterpret_cast<long*>(&object);
			bcc::intptr_t e = expected;
			return e == (expected = ::_InterlockedCompareExchange(p, desired, e));
		}

		template<typename Atomic>
		static bool compareExchangeStrongImpl(Atomic& object, bcc::intptr_t& expected, bcc::intptr_t desired, typename bcc::enable_if<sizeof(Atomic) == 8>::type*) BICOMC_NOEXCEPT
		{
			__int64* p = static_cast<__int64*>(&object);
			bcc::intptr_t e = expected;
			return e == (expected = ::_InterlockedCompareExchange64(p, desired, e));
		}

		template<typename Atomic>
		static bool compareExchangeStrongImpl(Atomic& object, bcc::intptr_t& expected, bcc::intptr_t desired, ...) BICOMC_NOEXCEPT
		{
			static_assert(false, "can't support atomic compare exchange.");
		}

		static void store(atomic_intptr_t& object, bcc::intptr_t desired) BICOMC_NOEXCEPT
		{
			exchangeImpl(object, desired, 0);
		}

		static bcc::intptr_t load(atomic_intptr_t const& object) BICOMC_NOEXCEPT
		{
			return loadImpl(const_cast<atomic_intptr_t&>(object), 0);
		}

		static bcc::intptr_t exchange(atomic_intptr_t& object, bcc::intptr_t desired) BICOMC_NOEXCEPT
		{
			return exchangeImpl(object, desired, 0);
		}

		static bool compareExchangeStrong(atomic_intptr_t& object, bcc::intptr_t& expected, bcc::intptr_t desired) BICOMC_NOEXCEPT
		{
			return compareExchangeStrongImpl(object, expected, desired, 0);
		}
	};

	inline void atomic_store(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		MsvcAtomic::store(*pObject, desired);
	}

	inline bcc::intptr_t atomic_load(atomic_intptr_t const* pObject) BICOMC_NOEXCEPT
	{
		return MsvcAtomic::load(*pObject);
	}

	inline bcc::intptr_t atomic_exchange(atomic_intptr_t* pObject, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		return MsvcAtomic::exchange(*pObject, desired);
	}

	inline bool atomic_compare_exchange_strong(atomic_intptr_t* pObject, bcc::intptr_t* pExpected, bcc::intptr_t desired) BICOMC_NOEXCEPT
	{
		return MsvcAtomic::compareExchangeStrong(*pObject, *pExpected, desired);
	}

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_COMPILER_MSVC_ATOMIC_H__
