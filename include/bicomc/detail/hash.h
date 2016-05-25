#ifndef BICOMC_DETAIL_HASH_H__
#define BICOMC_DETAIL_HASH_H__

#include "config.h"

#include "../array.h"
#include "../stdint.h"

namespace bcc
{
namespace detail
{
	struct Hash
	{
		union
		{
			bcc::uint8_t raws[16];
			struct
			{
				bcc::uint64_t hash;
				bcc::uint64_t subhash;
			};
		};
	};
	static_assert(sizeof(Hash) == 16, "'Hash' size must equal 16.");

	inline bool operator==(Hash const& lhs, Hash const& rhs) BICOMC_NOEXCEPT
	{
		return lhs.hash == rhs.hash && lhs.subhash == rhs.subhash;
	}

	inline bool operator!=(Hash const& lhs, Hash const& rhs) BICOMC_NOEXCEPT
	{
		return lhs.hash != rhs.hash || lhs.subhash != rhs.subhash;
	}

	template<typename T>
	struct Hasher;
	//	static bcc::uint64_t hash()
	//	static bcc::uint64_t subhash()

	template<typename T>
	struct Hasher<T const>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::subhash())
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T volatile>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::subhash())
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T const volatile>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::subhash())
		{
			return Hasher<T>::subhash();
		}
	};

	template<typename T>
	struct Hasher<T&>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::subhash())
		{
			return Hasher<T>::subhash();
		}
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct Hasher<T&&>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::subhash())
		{
			return Hasher<T>::subhash();
		}
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	template<typename T>
	struct Hasher<T*>
	{
		static bcc::uint64_t hash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::hash();
		}

		static bcc::uint64_t subhash() BICOMC_NOEXCEPT_EX(Hasher<T>::hash())
		{
			return Hasher<T>::subhash();
		}
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_HASH_H__
