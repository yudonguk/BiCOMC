#ifndef BICOMC_DETAIL_OBJECT_FORWARD_H__
#define BICOMC_DETAIL_OBJECT_FORWARD_H__

#include "config.h"

#include <stdexcept>
#include <string>
#include <utility>

#include "../array.h"
#include "../memory.h"
#include "../stdint.h"
#include "../tuple.h"
#include "../type_traits.h"

#include "atomic.h"
#include "compatibility.h"
#include "hash.h"
#include "method_traits.h"
#include "object_helper.h"
#include "safe_static.h"
#include "signature.h"
#include "string_util.h"

namespace bcc
{
	class Object;

namespace detail
{
	struct ObjectCaster;

	template<typename Method, Method>
	struct MethodCallHelper;

	template<typename Method>
	struct MethodCallNoOverHelper;

	struct DefaultCallHelper
	{
		struct Helper;

		template<typename Interfaces, typename MethodType, typename Impl>
		static void overrideDestroy(Impl& impl);

		template<typename Interfaces, typename MethodType, typename Impl>
		static void overrideClone(Impl& impl);
	};

	template<typename T>
	struct LazyBase
	{
		typedef typename T::BiCOMC_Base__ type;
	};

	template<>
	struct LazyBase<bcc::Object>
	{
		typedef void type;
	};

	template<typename T>
	struct InheritanceDepth
	{
		static size_t const value = T::BiCOMC_Base__::BICOMC_INHERITANCE_DEPTH__ + 1;
	};

	template<>
	struct InheritanceDepth<bcc::Object>
	{
		static size_t const value = 0;
	};

	template<typename T>
	struct FunctionTypes
	{
		typedef typename T::BiCOMC_Function_Types__ type;
	};

	template<typename FunctionTypes, size_t size = bcc::tuple_size<FunctionTypes>::value>
	struct RawTable
	{
		typedef typename bcc::TupleCat<
			typename RawTable<FunctionTypes, size - 1>::type
			, bcc::array<void const*, bcc::tuple_size<typename bcc::tuple_element<size - 1, FunctionTypes>::type>::value>
		>::type type;
	};

	template<typename FunctionTypes>
	struct RawTable<FunctionTypes, 0>
	{
		typedef bcc::tuple<> type;
	};

	template<typename FunctionTypes>
	struct RawTable<FunctionTypes, 1>
	{
		typedef bcc::tuple<
			bcc::array<void const*, bcc::tuple_size<typename bcc::tuple_element<0, FunctionTypes>::type>::value>
		> type;
	};

	template<typename FunctionTypes, size_t depth = bcc::tuple_size<FunctionTypes>::value - 1>
	struct InterfaceInfoDeducer
	{
		static size_t const HASH_HOLD_SIZE = sizeof(Hash) / sizeof(bcc::uintptr_t) + (sizeof(Hash) % sizeof(bcc::uintptr_t) == 0 ? 0 : 1);

		typedef typename bcc::TupleCat<
			typename bcc::conditional<
				sizeof(Hash) <= sizeof(bcc::uintptr_t)
				, bcc::tuple<bcc::uintptr_t, bcc::uintptr_t>
				, typename bcc::tuple_cat_n<bcc::tuple<>, bcc::uintptr_t, HASH_HOLD_SIZE + 1>::type
			>::type
			, char const*
		>::type header;

		typedef typename bcc::tuple_cat_n<
			header
			, char const*
			, bcc::tuple_size<typename bcc::tuple_element<depth, FunctionTypes>::type>::value - 1
		>::type type;

		typedef bcc::array<void const*, bcc::tuple_size<type>::value> raw_type;

		template<size_t size = bcc::tuple_size<type>::value - bcc::tuple_size<header>::value, typename Dummy = void>
		struct Helper
		{
			static void init(raw_type& result)
			{
				Helper<size - 1>::init(result);
				typedef typename bcc::tuple_element<size, typename bcc::tuple_element<depth, FunctionTypes>::type>::type MITT;
				bcc::get<bcc::tuple_size<header>::value + size - 1>(result) = MITT::signature();
			}
		};

		template<typename Dummy>
		struct Helper<0, Dummy>
		{
			static void init(raw_type& result) {}
		};

		template<typename Dummy>
		struct Helper<1, Dummy>
		{
			static void init(raw_type& result)
			{
				typedef typename bcc::tuple_element<1, typename bcc::tuple_element<depth, FunctionTypes>::type>::type MITT;
				bcc::get<bcc::tuple_size<header>::value>(result) = MITT::signature();
			}
		};

		template<typename Interface>
		static void init(raw_type& result, char const* name)
		{
			Helper<>::init(result);

			size_t const count = bcc::tuple_size<type>::value - bcc::tuple_size<header>::value;

			Hash& hash = reinterpret_cast<Hash&>(bcc::get<0>(result));
			hash.hash = bcc::detail::Hasher<Interface>::hash();
			hash.subhash = bcc::detail::Hasher<Interface>::subhash();

			bcc::get<HASH_HOLD_SIZE>(result) = reinterpret_cast<void const*>(count);
			bcc::get<HASH_HOLD_SIZE + 1>(result) = name;
		}
	};

	template<typename FunctionTypes, size_t begin = 0, size_t end = bcc::tuple_size<FunctionTypes>::value>
	struct TableCopyHelper
	{
		static size_t const depth = begin;

		static void copy(FunctionTypes& result, void** table, bcc::Object const& object)
		{
			bcc::get<depth>(result) = ObjectHelper::ownTable<depth, FunctionTypes>(object);
			table[depth] = &bcc::get<depth>(result);
			TableCopyHelper<FunctionTypes, begin + 1, end>::copy(result, table, object);
		}
	};

	template<typename FunctionTypes, size_t end>
	struct TableCopyHelper<FunctionTypes, end, end>
	{
		static void copy(FunctionTypes& result, void** table, bcc::Object const& object) {}
	};

	template<typename Interface>
	struct TableHolder
	{
		static size_t const HEADER_SIZE = ObjectHelper::VFTABLE_HEADER_SIZE;

		typedef typename bcc::detail::FunctionTypes<Interface>::type FunctionTypes;
		static size_t const depth = bcc::tuple_size<FunctionTypes>::value - 1;

		typedef typename bcc::detail::RawTable<FunctionTypes>::type RawTable;

		template<size_t index = 0, size_t end = bcc::tuple_size<typename bcc::tuple_element<depth, RawTable>::type>::value>
		struct Helper
		{
			static void init(RawTable& result)
			{
				typedef typename bcc::tuple_element<index, typename bcc::tuple_element<depth, FunctionTypes>::type>::type FunctionType;
				typedef typename FunctionType::deducer deducer;

				typename deducer::helper* function = &bcc::detail::MethodCallNoOverHelper<typename deducer::member>::call;
				bcc::get<depth>(result)[index] = reinterpret_cast<void*>(function);
				
				Helper<index + 1, end>::init(result);
			}
		};

		template<size_t end>
		struct Helper<0, end>
		{
			static void init(RawTable& result)
			{
				// interface info
				Helper<1, end>::init(result);
			}
		};

		template<size_t end>
		struct Helper<end, end>
		{
			static void init(RawTable& result) {}
		};

		template<typename T>
		struct Initializer
		{
			Initializer(T& o) : object(o) {}

			void operator()(TableHolder& holder)
			{
				holder.init(object);
			}

			T& object;
		};

		TableHolder()
		{}

		template<typename T>
		static Initializer<T> initializer(T& object)
		{
			return Initializer<T>(object);
		}

		template<typename T>
		void init(T& object)
		{
			name = bcc::detail::Signature<typename bcc::remove_cv<T>::type>::to_utf8();
			InterfaceInfoDeducer<FunctionTypes>::template init<Interface>(info, name.c_str());

			TableCopyHelper<RawTable, 0, depth>::copy(table, vftable.data() + HEADER_SIZE, object);

			bcc::get<0>(bcc::get<depth>(table)) = info.data();
			Helper<>::init(table);

			vftable[0] = reinterpret_cast<void*>(depth); // depth
			vftable[1] = reinterpret_cast<void*>(BICOMC_VERSION); // version
			vftable[2] = 0; // next
			vftable[depth + HEADER_SIZE] = &bcc::get<depth>(table); // table
		}

		std::string name;
		typename InterfaceInfoDeducer<FunctionTypes>::raw_type info;
		RawTable table;
		bcc::array<void*, bcc::tuple_size<RawTable>::value + HEADER_SIZE> vftable;
	};

	template<typename Interfaces>
	struct MultiTableHolder
	{
		static size_t const HEADER_SIZE = ObjectHelper::VFTABLE_HEADER_SIZE;

		template<size_t size = bcc::tuple_size<Interfaces>::value, typename Dummy = void>
		struct InterfaceTester
		{
			static bool const value = bcc::is_interface<typename bcc::tuple_element<size - 1, Interfaces>::type>::value
				&& InterfaceTester<size - 1>::value;
		};

		template<typename Dummy>
		struct InterfaceTester<0, Dummy>
		{
			static bool const value = true;
		};

		static_assert(InterfaceTester<>::value, "'Interfaces' must be a list of interface.");

		template<size_t size = bcc::tuple_size<Interfaces>::value, typename Dummy = void>
		struct DuplicationTester
		{
			typedef typename bcc::tuple_element<size - 1, Interfaces>::type Interface;

			static size_t const now = size;

			template<size_t count = bcc::tuple_size<Interfaces>::value, typename Dummy2 = void>
			struct Helper
			{
				typedef bcc::int8_t TrueType;
				typedef bcc::int16_t FalseType;

				static TrueType test(typename bcc::tuple_element<count - 1, Interfaces>::type*);
				static FalseType test(...);

				static bool const value =
					(count != now && sizeof(test(static_cast<Interface*>(0))) == sizeof(TrueType))
					|| Helper<count - 1>::value;
			};

			template<typename Dummy2>
			struct Helper<0, Dummy2>
			{
				static bool const value = false;
			};

			static bool const value = Helper<>::value || DuplicationTester<size - 1>::value;
		};

		template<typename Dummy>
		struct DuplicationTester<0, Dummy>
		{
			static bool const value = false;
		};

		static_assert(!DuplicationTester<>::value, "'Interfaces' must not be duplicated.");

		template<size_t size = bcc::tuple_size<Interfaces>::value, typename Dummy = void>
		struct Helper
		{
			static size_t const max_size = bcc::tuple_size<Interfaces>::value;

			typedef typename bcc::tuple_element<size - 1, Interfaces>::type Interface;
			typedef typename bcc::detail::FunctionTypes<Interface>::type FunctionTypes;
			typedef typename bcc::detail::RawTable<FunctionTypes>::type RawTable;
			typedef bcc::array<void*, bcc::tuple_size<RawTable>::value + HEADER_SIZE> Vftable;
			typedef bcc::tuple<RawTable, Vftable> Holder;

			typedef typename bcc::TupleCat<typename Helper<size - 1>::type, Holder>::type type;

			template<typename H, typename T>
			static void init(H& holders, T& object)
			{
				Helper<size - 1>::init(holders, object);

				Interface& impl = static_cast<Interface&>(object);
				Holder& holder = bcc::get<size - 1>(holders);
				RawTable& table = bcc::get<0>(holder);
				Vftable& vftable = bcc::get<1>(holder);

				TableCopyHelper<RawTable>::copy(table, vftable.data() + HEADER_SIZE, impl);
				vftable[0] = reinterpret_cast<void*>(bcc::tuple_size<FunctionTypes>::value - 1); // depth
				vftable[1] = reinterpret_cast<void*>(BICOMC_VERSION); // version
				vftable[2] = reinterpret_cast<void*>(next(object)); // next
			}

			template<typename H, typename T>
			static void setVftable(T& object, H& holders)
			{
				Helper<size - 1>::setVftable(object, holders);

				Interface& impl = static_cast<Interface&>(object);
				Holder& holder = bcc::get<size - 1>(holders);
				Vftable& vftable = bcc::get<1>(holder);

				bcc::detail::ObjectHelper::setTable(impl, vftable.data());
			}

			template<typename T>
			static bcc::intptr_t next(T& object)
			{
				return nextImpl(object, typename bcc::conditional<size != max_size, bcc::true_type, bcc::false_type>::type());
			}

			template<typename T>
			static bcc::intptr_t nextImpl(T& object, bcc::true_type)
			{
				typedef typename Helper<size + 1>::Interface Next;
				return reinterpret_cast<char const*>(&static_cast<bcc::Object const&>(static_cast<Next const&>(object)))
					- reinterpret_cast<char const*>(&static_cast<bcc::Object const&>(static_cast<Interface const&>(object)));
			}

			template<typename T>
			static bcc::intptr_t nextImpl(T& object, bcc::false_type)
			{
				typedef typename Helper<1>::Interface Next;
				return reinterpret_cast<char const*>(&static_cast<bcc::Object const&>(static_cast<Next const&>(object)))
					- reinterpret_cast<char const*>(&static_cast<bcc::Object const&>(static_cast<Interface const&>(object)));
			}
		};

		template<typename Dummy>
		struct Helper<0, Dummy>
		{
			typedef bcc::tuple<> type;

			template<typename H, typename T>
			static void init(H& holder, T& object) {}

			template<typename H, typename T>
			static void setVftable(T& object, H& holders) {}
		};

		template<typename T>
		struct Initializer
		{
			Initializer(T& o) : object(o) {}

			void operator()(MultiTableHolder& holder)
			{
				holder.init(object, holder);
			}

			T& object;
		};

		MultiTableHolder()
		{}

		template<typename T>
		static Initializer<T> initializer(T& object)
		{
			return Initializer<T>(object);
		}

		template<typename T>
		static void init(T& object, MultiTableHolder& holder)
		{
			holder.init(object);
			holder.setVftable(object);
			if (!object.BiCOMC_Override_Method_Helper__(bcc::true_type()))
				throw std::runtime_error("overriding fail");
		}

		template<typename T>
		void init(T& object)
		{
			Helper<>::init(holder, object);
		}

		template<typename T>
		void setVftable(T& object)
		{
			Helper<>::setVftable(object, holder);
		}

		typename Helper<>::type holder;
	};

	template<template<size_t, typename> class Enumerator, size_t line = 0, size_t index = 0>
	struct EnumeratorSize
	{
		typedef bcc::int8_t TrueType;
		typedef bcc::int16_t FalseType;

		template<size_t size>
		struct EnumeratorEnd
		{
			static size_t const value = size;
		};

		template<typename U> static TrueType test(typename U::type*);
		template<typename U> static FalseType test(...);

		static size_t const value
			= bcc::conditional<sizeof(test<Enumerator<index, void> >(0)) == sizeof(TrueType)
				, EnumeratorSize<Enumerator, line, index + 1>, EnumeratorEnd<index> >::type::value;
	};

	class OverrideInitHolder
	{
	public:
		OverrideInitHolder(bool isSuccess)
		{
			if (!isSuccess) throw std::runtime_error("overriding fail");
		}

	private:
		OverrideInitHolder(OverrideInitHolder const&) BICOMC_DELETE;
		OverrideInitHolder& operator=(OverrideInitHolder const&) BICOMC_DELETE;
	};

	template<typename T>
	struct Hasher
	{
		template<typename U = T, typename Dummy = void>
		struct HashHelper
		{
			static void list(std::string& result)
			{
				typedef typename bcc::detail::LazyBase<U>::type Base;
				HashHelper<Base>::list(result);
				result.append(1, static_cast<char>(10));
				result.append(bcc::detail::Signature<U>::to_utf8());
			}
		};

		template<typename Dummy>
		struct HashHelper<bcc::Object, Dummy>
		{
			static void list(std::string& result)
			{
				result = bcc::detail::Signature<bcc::Object>::to_utf8();
			}
		};

		typedef typename bcc::detail::FunctionTypes<T>::type FunctionTypes;
		static size_t const depth = bcc::detail::InheritanceDepth<T>::value;

		template<size_t size = bcc::tuple_size<typename bcc::tuple_element<depth, FunctionTypes>::type>::value, typename Dummy = void>
		struct SubhashHelper
		{
			static void list(std::string& result)
			{
				SubhashHelper<size - 1>::list(result);

				typedef typename bcc::tuple_element<size - 1, typename bcc::tuple_element<depth, FunctionTypes>::type>::type MITT;
				result.append(MITT::signature());

				if (bcc::tuple_size<typename bcc::tuple_element<depth, FunctionTypes>::type>::value != size)
					result.append(1, static_cast<char>(10));
			}
		};

		template<typename Dummy>
		struct SubhashHelper<1, Dummy>
		{
			static void list(std::string& result)
			{
				result.clear();
			}
		};

		static bcc::uint64_t hash()
		{
			std::string data;
			HashHelper<>::list(data);

			return murmurHashNeutral64v2(data.c_str(), data.size(), 0);
		}

		static bcc::uint64_t subhash()
		{
			std::string data;
			SubhashHelper<>::list(data);

			return murmurHashNeutral64v2(data.c_str(), data.size(), 0);
		}

		static bcc::uint64_t getBlock(bcc::uint8_t const* data) BICOMC_NOEXCEPT
		{
			return bcc::uint64_t(data[0])
				| bcc::uint64_t(data[1]) << 8
				| bcc::uint64_t(data[2]) << 16
				| bcc::uint64_t(data[3]) << 24
				| bcc::uint64_t(data[4]) << 32
				| bcc::uint64_t(data[5]) << 40
				| bcc::uint64_t(data[6]) << 48
				| bcc::uint64_t(data[7]) << 56;
		}

		static bcc::uint64_t rotl64(bcc::uint64_t x, int r) BICOMC_NOEXCEPT
		{
			return (x << r) | (x >> (64 - r));
		}
		
		static bcc::uint64_t fmix(bcc::uint64_t k) BICOMC_NOEXCEPT
		{
			k ^= k >> 33;
			k *= UINT64_C(0xFF51AFD7ED558CCD);
			k ^= k >> 33;
			k *= UINT64_C(0xC4CEB9FE1A85EC53);
			k ^= k >> 33;
			return k;
		}

		static bcc::uint64_t murmurHashNeutral64v2(void const* key, size_t len, bcc::uint64_t seed) BICOMC_NOEXCEPT
		{
			bcc::uint64_t const m = UINT64_C(0xC6A4A7935BD1E995);
			int const r = 47;

			bcc::uint64_t h = seed ^ (len * m);

			bcc::uint8_t const* data = static_cast<bcc::uint8_t const*>(key);
			bcc::uint8_t const* end = data + (len - len % sizeof(bcc::uint64_t));

			for (; data != end; data += sizeof(bcc::uint64_t))
			{
				bcc::uint64_t k = getBlock(data);

				k *= m;
				k ^= k >> r;
				k *= m;

				h ^= k;
				h *= m;
			}

			switch (len & 7)
			{
			case 7: h ^= bcc::uint64_t(data[6]) << 48;
			case 6: h ^= bcc::uint64_t(data[5]) << 40;
			case 5: h ^= bcc::uint64_t(data[4]) << 32;
			case 4: h ^= bcc::uint64_t(data[3]) << 24;
			case 3: h ^= bcc::uint64_t(data[2]) << 16;
			case 2: h ^= bcc::uint64_t(data[1]) << 8;
			case 1: h ^= bcc::uint64_t(data[0]);
				h *= m;
			};

			h ^= h >> r;
			h *= m;
			h ^= h >> r;

			return h;
		}

		static bcc::array<bcc::uint64_t, 2> murmurHashNeutral128v3(void const* key, size_t len, bcc::uint64_t seed) BICOMC_NOEXCEPT
		{
			bcc::uint8_t const* data = static_cast<bcc::uint8_t const*>(key);
			bcc::uint8_t const* end = data + (len - len % 16);

			bcc::uint64_t h1 = seed;
			bcc::uint64_t h2 = seed;

			bcc::uint64_t const c1 = UINT64_C(0x87C37B91114253D5);
			bcc::uint64_t const c2 = UINT64_C(0x4CF5AD432745937F);
			
			for (; data != end; data += 16)
			{
				bcc::uint64_t k1 = getBlock(data);
				bcc::uint64_t k2 = getBlock(data + sizeof(bcc::uint64_t));

				k1 *= c1;
				k1 = rotl64(k1, 31);
				k1 *= c2;

				h1 ^= k1;
				h1 = rotl64(h1, 27);
				h1 += h2;
				h1 = h1 * 5 + UINT32_C(0x52DCE729);

				k2 *= c2;
				k2 = rotl64(k2, 33);
				k2 *= c1;

				h2 ^= k2;
				h2 = rotl64(h2, 31);
				h2 += h1;
				h2 = h2 * 5 + UINT32_C(0x38495AB5);
			}

			bcc::uint64_t k1 = 0, k2 = 0;

			switch (len & 15)
			{
			case 15: k2 ^= bcc::uint64_t(data[14]) << 48;
			case 14: k2 ^= bcc::uint64_t(data[13]) << 40;
			case 13: k2 ^= bcc::uint64_t(data[12]) << 32;
			case 12: k2 ^= bcc::uint64_t(data[11]) << 24;
			case 11: k2 ^= bcc::uint64_t(data[10]) << 16;
			case 10: k2 ^= bcc::uint64_t(data[9]) << 8;
			case 9: k2 ^= bcc::uint64_t(data[8]);
				k2 *= c2;
				k2 = rotl64(k2, 33);
				k2 *= c1;
				h2 ^= k2;
					
			case 8: k1 ^= bcc::uint64_t(data[7]) << 56;
			case 7: k1 ^= bcc::uint64_t(data[6]) << 48;
			case 6: k1 ^= bcc::uint64_t(data[5]) << 40;
			case 5: k1 ^= bcc::uint64_t(data[4]) << 32;
			case 4: k1 ^= bcc::uint64_t(data[3]) << 24;
			case 3: k1 ^= bcc::uint64_t(data[2]) << 16;
			case 2: k1 ^= bcc::uint64_t(data[1]) << 8;
			case 1: k1 ^= bcc::uint64_t(data[0]);
				k1 *= c1;
				k1 = rotl64(k1, 31);
				k1 *= c2;
				h1 ^= k1;
			};

			h1 ^= len;
			h2 ^= len;

			h1 += h2;
			h2 += h1;

			h1 = fmix(h1);
			h2 = fmix(h2);

			h1 += h2;
			h2 += h1;
	
			bcc::array<bcc::uint64_t, 2> result;
			result[0] = h1;
			result[1] = h2;
			return result;
		}

		static bcc::uint64_t fnv1a64(void const* key, size_t len, bcc::uint64_t seed) BICOMC_NOEXCEPT
		{
			bcc::uint8_t const* data = static_cast<bcc::uint8_t const*>(key);
			bcc::uint8_t const* end = data + len;

			bcc::uint64_t h = seed;
			
			for (; data != end; ++data)
			{
				h ^= bcc::uint64_t(*data);
				h *= UINT64_C(0x100000001B3);
			}

			return h;
		}
	};
} // namespace detail

namespace detail
{
	template<typename T>
	struct is_interface_impl
	{
		typedef bcc::int8_t TrueType;
		typedef bcc::int16_t FalseType;

		template<typename U> static TrueType test(typename U::BiCOMC_Base__*);
		template<typename U> static FalseType test(...);

		template<typename U> static TrueType test2(typename U::BiCOMC_My__*);
		template<typename U> static FalseType test2(...);

		template<typename U> static TrueType test3(typename U::BiCOMC_Function_Types__*);
		template<typename U> static FalseType test3(...);

		template<typename U> struct Helper
		{
			typedef InheritanceDepth<U> type;
		};

		template<typename U> static TrueType test4(typename Helper<U>::type*);
		template<typename U> static FalseType test4(...);

		template<typename U>
		struct Tester
		{
			typedef typename U::BiCOMC_Base__ Base;
			typedef typename U::BiCOMC_My__ My;

			static bool const value	= bcc::is_same<U, My>::value
				&& InheritanceDepth<Base>::value != InheritanceDepth<My>::value;
		};

		static bool const value = bcc::conditional<
				sizeof(test<T>(0)) == sizeof(TrueType)
				&& sizeof(test2<T>(0)) == sizeof(TrueType)
				&& sizeof(test3<T>(0)) == sizeof(TrueType)
				&& sizeof(test4<T>(0)) == sizeof(TrueType)
				, Tester<T>
				, bcc::false_type
			>::type::value;
	};

	template<>
	struct is_interface_impl<bcc::Object>
	{
		static bool const value = true;
	};
} // namespace detail

	template<typename T>
	struct is_interface : public bcc::integral_constant<bool, detail::is_interface_impl<typename bcc::remove_cv<T>::type>::value>
	{};

	inline bool is_multiple(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		return bcc::detail::ObjectHelper::isMultiple(object);
	}

	inline bool is_multiple(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		return bcc::detail::ObjectHelper::isMultiple(object);
	}

	template<typename Target>
	bool is_compatible(bcc::Object const& object) BICOMC_NOEXCEPT
	{
		static_assert(bcc::is_interface<Target>::value, "'Target' type must be interface.");
		class Temp : public bcc::remove_cv<Target>::type {} target;
		return bcc::detail::ObjectHelper::isCompatible(object, target);
	}

	template<typename Target>
	bool is_compatible(bcc::Object const volatile& object) BICOMC_NOEXCEPT
	{
		static_assert(bcc::is_interface<Target>::value, "'Target' type must be interface.");
		class Temp : public bcc::remove_cv<Target>::type {} target;
		return bcc::detail::ObjectHelper::isCompatible(object, target);
	}

	template<typename T, typename U>
	T bicomc_cast(U& object);

	template<typename T, typename U>
	T bicomc_cast(U* object) BICOMC_NOEXCEPT;
} // namespace bcc

using bcc::bicomc_cast;

#define BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME) \
	friend class bcc::detail::ObjectHelper; \
	template<typename> friend struct bcc::detail::is_interface_impl; \
	template<typename> friend struct bcc::detail::InheritanceDepth; \
	template<typename> friend struct bcc::detail::LazyBase; \
	template<typename> friend struct bcc::detail::Signature; \
protected: \
	static size_t const BICOMC_INHERITANCE_DEPTH__ = bcc::detail::InheritanceDepth<INTERFACE_NAME >::value; \
private: \
	typedef INTERFACE_NAME BiCOMC_My__; \
	\
	BICOMC_SIGNATURE_DEFAULT(INTERFACE_NAME) \
	\
	template<size_t BiCOMC_index__, typename BiCOMC_Dummy__ = void> \
	struct BiCOMC_Type_Enumerator__; \
	template<typename BiCOMC_Dummy__> \
	struct BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, BICOMC_LINE_COUNTER>::value, BiCOMC_Dummy__> \
	{ \
		typedef void* type; \
		typedef bcc::tuple<type> list; \
	}

#define BICOMC_INTERFACE_DERIVED(INTERFACE_NAME, BASE_NAME) \
class INTERFACE_NAME : public BASE_NAME \
{ \
private: \
	typedef BASE_NAME BiCOMC_Base__; \
	BICOMC_STATIC_ASSERT((bcc::is_interface<BiCOMC_Base__>::value), "'" #BASE_NAME "' must be interface.", interface_base_check); \
	BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME);

#define BICOMC_INTERFACE(INTERFACE_NAME, ...) \
BICOMC_STATIC_ASSERT((bcc::tuple_size<bcc::tuple<__VA_ARGS__ > >::value <= 1), "BICOMC_INTERFACE() has one or two parameters.", interface_parameter_check); \
class INTERFACE_NAME : public bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__ >, bcc::Object>::type>::type \
{ \
private: \
	typedef bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__ >, bcc::Object>::type>::type BiCOMC_Base__; \
	BICOMC_STATIC_ASSERT((bcc::is_interface<BiCOMC_Base__>::value), "Base type must be interface.", interface_base_check); \
	BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME);

#define BICOMC_INTERFACE_FUNCTION_TABLE(INTERFACE_NAME) \
private: \
	template<typename BiCOMC_Owner__, typename BiCOMC_Dummy__ = void> \
	struct BiCOMC_Type_Decider__ \
	{ \
		typedef BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, BICOMC_LINE_COUNTER>::value - 1>::list list; \
		typedef typename bcc::detail::InterfaceInfoDeducer<bcc::tuple<list> >::type info_type; \
		typedef typename bcc::TupleCat< \
			typename BiCOMC_Owner__::BiCOMC_Base__::BiCOMC_Function_Types__ \
			, typename bcc::tuple_replace<list, 0, info_type>::type \
		>::type type; \
	}; \
	template<typename BiCOMC_Dummy__> \
	struct BiCOMC_Type_Decider__<bcc::Object, BiCOMC_Dummy__> \
	{ \
		typedef BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, BICOMC_LINE_COUNTER>::value - 1>::list list; \
		typedef typename bcc::detail::InterfaceInfoDeducer<bcc::tuple<list> >::type info_type; \
		typedef bcc::tuple<typename bcc::tuple_replace<list, 0, info_type>::type> type; \
	}; \
protected: \
	template<typename> friend struct bcc::detail::FunctionTypes; \
	typedef BiCOMC_Type_Decider__<INTERFACE_NAME >::type BiCOMC_Function_Types__;

#define BICOMC_INTERFACE_END(INTERFACE_NAME) \
protected: \
	BICOMC_INTERFACE_FUNCTION_TABLE(INTERFACE_NAME); \
protected: \
	INTERFACE_NAME() \
		: BiCOMC_Base__() \
	{ \
		typedef bcc::detail::TableHolder<INTERFACE_NAME> Holder; \
		typedef bcc::detail::SafeStatic<Holder, INTERFACE_NAME> StaticHolder; \
		\
		Holder* pHolder = StaticHolder::get(); \
		if (!pHolder) pHolder = StaticHolder::init(Holder::initializer(*this)); \
		bcc::detail::ObjectHelper::setTable(*this, pHolder->vftable.data()); \
	} \
	~INTERFACE_NAME() {} \
private: \
	INTERFACE_NAME(BiCOMC_My__ const& rhs) BICOMC_DELETE; \
};

#define BICOMC_METHOD_TYPE_NAME(METHOD_NAME) \
	BiCOMC_ ## METHOD_NAME ## _Type__

#define BICOMC_OPERATOR_UNARY_PLUS BiCOMC_Operator_Unary_Plus__ // operator+
#define BICOMC_OPERATOR_UNARY_MINUS BiCOMC_Operator_Unary_Minus__ // operator-
#define BICOMC_OPERATOR_PLUS BiCOMC_Operator_Plus__ // operator+
#define BICOMC_OPERATOR_MINUS BiCOMC_Operator_Minus__ // operator-
#define BICOMC_OPERATOR_DEREFERENCE BiCOMC_Operator_Dereference__ // operator*
#define BICOMC_OPERATOR_PRODUCT BiCOMC_Operator_Product__ // operator*
#define BICOMC_OPERATOR_DIVIDE BiCOMC_Operator_Divide__ // operator/
#define BICOMC_OPERATOR_MODULO BiCOMC_Operator_Modulo__ // operator%
#define BICOMC_OPERATOR_BITWISE_XOR BiCOMC_Operator_Bitwise_Xor__ // operator^
#define BICOMC_OPERATOR_ADDRESS_OF BiCOMC_Operator_Address_Of__ // operator&
#define BICOMC_OPERATOR_BITWISE_AND BiCOMC_Operator_Bitwise_And__ // operator&
#define BICOMC_OPERATOR_BITWISE_OR BiCOMC_Operator_Bitwise_Or__ // operator|
#define BICOMC_OPERATOR_BITWISE_NOT BiCOMC_Operator_Bitwise_Not__ // operator~
#define BICOMC_OPERATOR_LOGICAL_NOT BiCOMC_Operator_Logical_Not__// operator!
#define BICOMC_OPERATOR_ASSIGN BiCOMC_Operator_Assign__ // operator=
#define BICOMC_OPERATOR_LESS BiCOMC_Operator_Less__ // operator<
#define BICOMC_OPERATOR_GREATER BiCOMC_Operator_Greater__ // operator>
#define BICOMC_OPERATOR_PLUS_ASSIGN BiCOMC_Operator_Plus_Assign__ // operator+=
#define BICOMC_OPERATOR_MINUS_ASSIGN BiCOMC_Operator_Minus_Assign__ // operator-=
#define BICOMC_OPERATOR_PRODUCT_ASSIGN BiCOMC_Operator_Product_Assign__ // operator*=
#define BICOMC_OPERATOR_DIVIDE_ASSIGN BiCOMC_Operator_Divide_Assign__ // operator/=
#define BICOMC_OPERATOR_MODULO_ASSIGN BiCOMC_Operator_Modulo_Assign__ // operator%=
#define BICOMC_OPERATOR_BITWISE_XOR_ASSIGN BiCOMC_Operator_Bitwise_Xor_Assign__ // operator^=
#define BICOMC_OPERATOR_BITWISE_AND_ASSIGN BiCOMC_Operator_Bitwise_And_Assign__ // operator&=
#define BICOMC_OPERATOR_BITWISE_OR_ASSIGN BiCOMC_Operator_Bitwise_Or_Assign__ // operator|=
#define BICOMC_OPERATOR_LEFT_SHIFT BiCOMC_Operator_Left_Shift__ // operator<<
#define BICOMC_OPERATOR_RIGHT_SHIFT BiCOMC_Operator_Right_Shift__ // operator>>
#define BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN BiCOMC_Operator_Left_Shift_Assign__ // operator<<=
#define BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN BiCOMC_Operator_Right_Shift_Assign__ // operator>>=
#define BICOMC_OPERATOR_EQUAL BiCOMC_Operator_Equal__ // operator==
#define BICOMC_OPERATOR_NOT_EQUAL BiCOMC_Operator_Not_Equal__ // operator!=
#define BICOMC_OPERATOR_LESS_EQUAL BiCOMC_Operator_Less_Equal__ // operator<=
#define BICOMC_OPERATOR_GREATER_EQUAL BiCOMC_Operator_Greater_Equal__ // operator>=
#define BICOMC_OPERATOR_LOGICAL_AND BiCOMC_Operator_Logical_And__ // operator&&
#define BICOMC_OPERATOR_LOGICAL_OR BiCOMC_Operator_Logical_Or__ // operator||
#define BICOMC_OPERATOR_COMMA BiCOMC_Operator_Comma__ // operator,
#define BICOMC_OPERATOR_PREFIX_INCREMENT BiCOMC_Operator_Prefix_Increment__ // operator++
#define BICOMC_OPERATOR_POSTFIX_INCREMENT BiCOMC_Operator_Postfix_Increment__ // operator++
#define BICOMC_OPERATOR_PREFIX_DECREMENT BiCOMC_Operator_Prefix_Decrement__ // operator--
#define BICOMC_OPERATOR_POSTFIX_DECREMENT BiCOMC_Operator_Postfix_Decrement__ // operator--
#define BICOMC_OPERATOR_POINTER_TO_MEMBER BiCOMC_Operator_Pointer_To_Memeber__ // operator->*
#define BICOMC_OPERATOR_STRUCTURE_DEREFERENCE BiCOMC_Operator_Structure_Dereference__ // operator->
#define BICOMC_OPERATOR_SUBSCRIPT BiCOMC_Operator_Subscript__ // operator[]
#define BICOMC_OPERATOR_CALL BiCOMC_Operator_Call__ // operator()

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#	define BICOMC_METHOD_TYPE_DEF_SIGNATURE_STRING(...) \
		std::string(BICOMC_STRINGIZER(__VA_ARGS__)) \
			.append(1, char(61)) /* 61 == u8'=' */ \
			.append(bcc::detail::Signature<deducer>::to_utf8())
#else
#	define BICOMC_METHOD_TYPE_DEF_SIGNATURE_STRING(...) \
		bcc::detail::StringUtil::convertToUtf8(BICOMC_WSTRINGIZER(__VA_ARGS__)) \
			.append(1, char(61)) /* 61 == u8'=' */ \
			.append(bcc::detail::Signature<deducer>::to_utf8())
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

#define BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
protected: \
	template<typename BiCOMC_Function__, bool BiCOMC_is_const__, bool BiCOMC_is_volatile__, typename BiCOMC_Dummy__> struct BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME); \
	template<typename BiCOMC_Dummy__> \
	struct BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, BiCOMC_Dummy__> \
	{ \
		typedef BiCOMC_My__ METHOD_QUALIFIER owner; \
		typedef bcc::detail::MethodTypeDeducer<METHOD_TYPE, owner> deducer; \
		typedef typename deducer::trait trait; \
		\
		static bool const isConst = bcc::is_const<int METHOD_QUALIFIER>::value; \
		static bool const isVolatile = bcc::is_volatile<int METHOD_QUALIFIER>::value; \
		static size_t const depth = owner::BICOMC_INHERITANCE_DEPTH__; \
		static size_t const index = bcc::detail::EnumeratorSize<owner::BiCOMC_Type_Enumerator__, BICOMC_LINE_COUNTER>::value; \
		\
		template<typename BiCOMC_Interfaces__, size_t BiCOMC_size__ = bcc::tuple_size<BiCOMC_Interfaces__>::value> \
		struct Helper \
		{ \
			typedef typename bcc::tuple_element<BiCOMC_size__ - 1, BiCOMC_Interfaces__>::type Interface; \
			\
			template<typename BiCOMC_Impl__> \
			static void overrideMethod(BiCOMC_Impl__& impl) \
			{ \
				overrideMethodImpl<Interface>(impl, 0); \
				Helper<BiCOMC_Interfaces__, BiCOMC_size__ - 1>::template overrideMethod(impl); \
			} \
			template<typename BiCOMC_U__, typename BiCOMC_Impl__> \
			static void overrideMethodImpl(BiCOMC_Impl__& impl, typename BiCOMC_U__::template BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, isConst, isVolatile, BiCOMC_Dummy__>* p) \
			{ \
				access(impl, p); \
			} \
			template<typename MethodType, typename BiCOMC_Impl__> \
			static void access(BiCOMC_Impl__& impl, MethodType*) \
			{ \
				MethodType::template overrideMethod<Interface>(impl); \
			} \
			template<typename BiCOMC_U__, typename BiCOMC_Impl__> \
			static void overrideMethodImpl(BiCOMC_Impl__& impl, ...) {} \
		}; \
		template<typename BiCOMC_Interfaces__> \
		struct Helper<BiCOMC_Interfaces__, 0> \
		{ \
			template<typename BiCOMC_Impl__> static void overrideMethod(BiCOMC_Impl__& impl) {} \
		}; \
		\
		static typename deducer::helper*& function(owner& impl) \
		{ \
			return bcc::detail::ObjectHelper::function<index, depth, owner::BiCOMC_Function_Types__>(impl); \
		} \
		template<typename BiCOMC_U__, typename BiCOMC_Impl__> \
		static typename bcc::enable_if<bcc::is_interface<BiCOMC_U__>::value>::type overrideMethod(BiCOMC_Impl__& impl) \
		{ \
			function(static_cast<BiCOMC_U__&>(impl)) = &bcc::detail::MethodCallHelper<typename deducer::template change_owner<BiCOMC_Impl__>::member, &BiCOMC_Impl__::METHOD_NAME >::template call<owner, BiCOMC_U__>; \
		} \
		template<typename BiCOMC_U__, typename BiCOMC_Impl__> \
		static typename bcc::enable_if<bcc::is_interface<BiCOMC_U__>::value>::type overrideMethod(BiCOMC_Impl__& impl, typename deducer::helper* func) \
		{ \
			function(static_cast<BiCOMC_U__&>(impl)) = func; \
		} \
		template<typename BiCOMC_Interfaces__, typename BiCOMC_Impl__> \
		static typename bcc::enable_if<!bcc::is_interface<BiCOMC_Interfaces__>::value>::type overrideMethod(BiCOMC_Impl__& impl) \
		{ \
			Helper<BiCOMC_Interfaces__>::template overrideMethod(impl); \
		} \
		\
		static char const* signature() \
		{ \
			typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, isConst, isVolatile, BiCOMC_Dummy__> MethodType; \
			typedef bcc::detail::SafeStatic<std::string, MethodType> StaticHolder; \
			std::string* pSignature = StaticHolder::get(); \
			if (!pSignature) pSignature = StaticHolder::init(&MethodType::getSignature); \
			return pSignature->c_str(); \
		} \
		static void getSignature(std::string& result) \
		{ \
			result = BICOMC_METHOD_TYPE_DEF_SIGNATURE_STRING(METHOD_NAME); \
		} \
	}; \
private: \
	template<typename BiCOMC_Dummy__> \
	struct BiCOMC_Type_Enumerator__<BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void>::index, BiCOMC_Dummy__> \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MethodType; \
		typedef MethodType type; \
		typedef typename bcc::TupleCat<typename BiCOMC_Type_Enumerator__<MethodType::index - 1>::list, type>::type list; \
	};

#define BICOMC_METHOD_TYPE_CHECKER(INTERFACE_NAME) \
	BiCOMC_ ## INTERFACE_NAME ## _Type_Checker__

#define BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
private: \
	template<typename BiCOMC_T__, typename BiCOMC_Function__, bool BiCOMC_is_const__, bool BiCOMC_is_volatile__> struct BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME); \
	template<typename BiCOMC_T__> \
	struct BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<BiCOMC_T__, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value> \
	{ \
		typedef bcc::int8_t TrueType; \
		typedef bcc::int16_t FalseType; \
		template<template<typename, bool, bool, typename> class BiCOMC_U__> \
		struct Helper { typedef BiCOMC_U__<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> type; }; \
		template<typename BiCOMC_U__> static TrueType test(typename Helper<BiCOMC_U__::template BICOMC_METHOD_TYPE_NAME(METHOD_NAME)>::type::owner*); \
		template<typename BiCOMC_U__> static FalseType test(...); \
		static bool const value = sizeof(test<BiCOMC_T__>(0)) == sizeof(TrueType); \
	};

#define BICOMC_OVER_METHOD(METHOD_NAME, METHOD_TYPE) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, false, void>::template overrideMethod<BiCOMC_Interfaces__>(*this); \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check);

#define BICOMC_OVER_METHOD_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, false, void>::template overrideMethod<BiCOMC_Interfaces__>(*this); \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check);

#define BICOMC_OVER_METHOD_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, true, void>::template overrideMethod<BiCOMC_Interfaces__>(*this); \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check);

#define BICOMC_OVER_METHOD_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, true, void>::template overrideMethod<BiCOMC_Interfaces__>(*this); \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check);

#define BICOMC_OVER_DESTROY() \
	bcc::detail::DefaultCallHelper::template overrideDestroy<BiCOMC_Interfaces__, bcc::Object::BICOMC_METHOD_TYPE_NAME(destroy)<void(), true, true, void> >(*this);

#define BICOMC_OVER_CLONE() \
	bcc::detail::DefaultCallHelper::template overrideClone<BiCOMC_Interfaces__, bcc::Object::BICOMC_METHOD_TYPE_NAME(clone)<bcc::Object*(), true, false, void> >(*this);

#define BICOMC_OVER_OPERATOR_UNARY_PLUS(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_UNARY_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_PLUS_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_UNARY_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_PLUS_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_UNARY_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_PLUS_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_UNARY_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_MINUS(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_UNARY_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_MINUS_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_UNARY_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_MINUS_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_UNARY_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_UNARY_MINUS_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_UNARY_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PLUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_MINUS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DEREFERENCE(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DEREFERENCE_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DEREFERENCE_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DEREFERENCE_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PRODUCT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PRODUCT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PRODUCT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PRODUCT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_DIVIDE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_DIVIDE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_DIVIDE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_DIVIDE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_MODULO, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_MODULO, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_MODULO, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_MODULO, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_XOR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_XOR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_XOR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_XOR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ADDRESS_OF(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_ADDRESS_OF, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ADDRESS_OF_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_ADDRESS_OF, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ADDRESS_OF_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_ADDRESS_OF, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ADDRESS_OF_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_ADDRESS_OF, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_NOT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_NOT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_NOT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_NOT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_NOT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LOGICAL_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_NOT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LOGICAL_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_NOT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LOGICAL_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_NOT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LOGICAL_NOT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LESS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LESS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LESS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LESS, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_GREATER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_GREATER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_GREATER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_GREATER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PLUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PLUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PLUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PLUS_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PLUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_MINUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_MINUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_MINUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MINUS_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_MINUS_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PRODUCT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PRODUCT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PRODUCT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PRODUCT_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PRODUCT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_DIVIDE_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_DIVIDE_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_DIVIDE_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_DIVIDE_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_DIVIDE_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_MODULO_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_MODULO_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_MODULO_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_MODULO_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_MODULO_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_XOR_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_AND_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_AND_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_AND_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_AND_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_AND_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_BITWISE_OR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_BITWISE_OR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_BITWISE_OR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_BITWISE_OR_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_BITWISE_OR_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LEFT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LEFT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LEFT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LEFT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_RIGHT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_RIGHT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_RIGHT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_RIGHT_SHIFT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LEFT_SHIFT_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_ASSIGN(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_ASSIGN_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_ASSIGN_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_RIGHT_SHIFT_ASSIGN_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_EQUAL(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_EQUAL_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_EQUAL_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_EQUAL_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_NOT_EQUAL(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_NOT_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_NOT_EQUAL_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_NOT_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_NOT_EQUAL_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_NOT_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_NOT_EQUAL_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_NOT_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_EQUAL(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LESS_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_EQUAL_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LESS_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_EQUAL_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LESS_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LESS_EQUAL_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LESS_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_EQUAL(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_GREATER_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_EQUAL_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_GREATER_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_EQUAL_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_GREATER_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_GREATER_EQUAL_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_GREATER_EQUAL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_AND(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LOGICAL_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_AND_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LOGICAL_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_AND_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LOGICAL_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_AND_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LOGICAL_AND, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_OR(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_LOGICAL_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_OR_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_LOGICAL_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_OR_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_LOGICAL_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_LOGICAL_OR_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_LOGICAL_OR, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_COMMA(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_COMMA, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_COMMA_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_COMMA, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_COMMA_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_COMMA, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_COMMA_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_COMMA, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_INCREMENT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PREFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_INCREMENT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PREFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_INCREMENT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PREFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_INCREMENT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PREFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_INCREMENT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_POSTFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_INCREMENT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_POSTFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_INCREMENT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_POSTFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_INCREMENT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_POSTFIX_INCREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_DECREMENT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_PREFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_DECREMENT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_PREFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_DECREMENT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_PREFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_PREFIX_DECREMENT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_PREFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_DECREMENT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_POSTFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_DECREMENT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_POSTFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_DECREMENT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_POSTFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POSTFIX_DECREMENT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_POSTFIX_DECREMENT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POINTER_TO_MEMBER(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_POINTER_TO_MEMBER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POINTER_TO_MEMBER_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_POINTER_TO_MEMBER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POINTER_TO_MEMBER_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_POINTER_TO_MEMBER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_POINTER_TO_MEMBER_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_POINTER_TO_MEMBER, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_STRUCTURE_DEREFERENCE(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_STRUCTURE_DEREFERENCE_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_STRUCTURE_DEREFERENCE_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_STRUCTURE_DEREFERENCE_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_SUBSCRIPT(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_SUBSCRIPT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_SUBSCRIPT_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_SUBSCRIPT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_SUBSCRIPT_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_SUBSCRIPT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_SUBSCRIPT_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_SUBSCRIPT, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_CALL(METHOD_TYPE) BICOMC_OVER_METHOD(BICOMC_OPERATOR_CALL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_CALL_C(METHOD_TYPE) BICOMC_OVER_METHOD_C(BICOMC_OPERATOR_CALL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_CALL_V(METHOD_TYPE) BICOMC_OVER_METHOD_V(BICOMC_OPERATOR_CALL, METHOD_TYPE)
#define BICOMC_OVER_OPERATOR_CALL_CV(METHOD_TYPE) BICOMC_OVER_METHOD_CV(BICOMC_OPERATOR_CALL, METHOD_TYPE)

#define BICOMC_OVERRIDE(...) \
private: \
	BICOMC_STATIC_ASSERT((bcc::tuple_size<bcc::tuple<__VA_ARGS__ > >::value != 0), "BICOMC_OVERRIDE() has one or more paramters.", override_parameter_check); \
	friend struct bcc::detail::DefaultCallHelper; \
	friend struct bcc::detail::MultiTableHolder<bcc::tuple<__VA_ARGS__ > >; \
	struct BiCOMC_Override_Method_Local__ {}; \
	\
	inline bool BiCOMC_Override_Method_Helper__() \
	{ \
		typedef bcc::detail::MultiTableHolder<bcc::tuple<__VA_ARGS__ > > Holder; \
		typedef bcc::detail::SafeStatic<Holder, BiCOMC_Override_Method_Local__> StaticHolder; \
		\
		Holder* pHolder = StaticHolder::get(); \
		if (!pHolder) pHolder = StaticHolder::init(Holder::initializer(*this)); \
		pHolder->setVftable(*this); \
		return true; \
	} \
	inline bool BiCOMC_Override_Method_Helper__(bcc::true_type /*isInitMode*/) \
	{ \
		typedef bcc::tuple<__VA_ARGS__ > BiCOMC_Interfaces__; \
		BICOMC_OVER_DESTROY();

#define BICOMC_OVERRIDE_END() \
		return true; \
	} \
	bcc::detail::OverrideInitHolder BiCOMC_override_init_holder__; \
public:

#define BICOMC_OVERRIDE_INIT() BiCOMC_override_init_holder__(BiCOMC_Override_Method_Helper__())

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME() METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_0(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_0_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_0_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_0_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_UNARY_PLUS(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator+, BICOMC_OPERATOR_UNARY_PLUS, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_PLUS_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator+, BICOMC_OPERATOR_UNARY_PLUS, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_PLUS_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator+, BICOMC_OPERATOR_UNARY_PLUS, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_PLUS_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator+, BICOMC_OPERATOR_UNARY_PLUS, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_MINUS(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator-, BICOMC_OPERATOR_UNARY_MINUS, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_MINUS_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator-, BICOMC_OPERATOR_UNARY_MINUS, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_MINUS_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator-, BICOMC_OPERATOR_UNARY_MINUS, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_UNARY_MINUS_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator-, BICOMC_OPERATOR_UNARY_MINUS, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DEREFERENCE(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator*, BICOMC_OPERATOR_DEREFERENCE, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DEREFERENCE_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator*, BICOMC_OPERATOR_DEREFERENCE, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DEREFERENCE_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator*, BICOMC_OPERATOR_DEREFERENCE, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DEREFERENCE_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator*, BICOMC_OPERATOR_DEREFERENCE, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ADDRESS_OF(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator&, BICOMC_OPERATOR_ADDRESS_OF, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ADDRESS_OF_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator&, BICOMC_OPERATOR_ADDRESS_OF, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ADDRESS_OF_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator&, BICOMC_OPERATOR_ADDRESS_OF, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ADDRESS_OF_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator&, BICOMC_OPERATOR_ADDRESS_OF, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_NOT(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator~, BICOMC_OPERATOR_BITWISE_NOT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_NOT_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator~, BICOMC_OPERATOR_BITWISE_NOT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_NOT_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator~, BICOMC_OPERATOR_BITWISE_NOT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_NOT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator~, BICOMC_OPERATOR_BITWISE_NOT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_NOT(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator!, BICOMC_OPERATOR_LOGICAL_NOT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_NOT_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator!, BICOMC_OPERATOR_LOGICAL_NOT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_NOT_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator!, BICOMC_OPERATOR_LOGICAL_NOT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_NOT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator!, BICOMC_OPERATOR_LOGICAL_NOT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_INCREMENT(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator++, BICOMC_OPERATOR_PREFIX_INCREMENT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_INCREMENT_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator++, BICOMC_OPERATOR_PREFIX_INCREMENT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_INCREMENT_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator++, BICOMC_OPERATOR_PREFIX_INCREMENT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_INCREMENT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator++, BICOMC_OPERATOR_PREFIX_INCREMENT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_DECREMENT(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator--, BICOMC_OPERATOR_PREFIX_DECREMENT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_DECREMENT_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator--, BICOMC_OPERATOR_PREFIX_DECREMENT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_DECREMENT_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator--, BICOMC_OPERATOR_PREFIX_DECREMENT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PREFIX_DECREMENT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator--, BICOMC_OPERATOR_PREFIX_DECREMENT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_STRUCTURE_DEREFERENCE(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator->, BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_STRUCTURE_DEREFERENCE_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator->, BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_STRUCTURE_DEREFERENCE_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator->, BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_STRUCTURE_DEREFERENCE_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator->, BICOMC_OPERATOR_STRUCTURE_DEREFERENCE, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_0(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_0_C(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_0_V(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_0_CV(METHOD_TYPE) BICOMC_DECL_METHOD_0_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(BICOMC_MACRO_PASS((METHOD_NAME)), METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_1(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_1_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_1_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_1_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_PLUS(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+, BICOMC_OPERATOR_PLUS, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+, BICOMC_OPERATOR_PLUS, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+, BICOMC_OPERATOR_PLUS, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+, BICOMC_OPERATOR_PLUS, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-, BICOMC_OPERATOR_MINUS, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-, BICOMC_OPERATOR_MINUS, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-, BICOMC_OPERATOR_MINUS, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-, BICOMC_OPERATOR_MINUS, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*, BICOMC_OPERATOR_PRODUCT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*, BICOMC_OPERATOR_PRODUCT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*, BICOMC_OPERATOR_PRODUCT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*, BICOMC_OPERATOR_PRODUCT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/, BICOMC_OPERATOR_DIVIDE, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/, BICOMC_OPERATOR_DIVIDE, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/, BICOMC_OPERATOR_DIVIDE, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/, BICOMC_OPERATOR_DIVIDE, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%, BICOMC_OPERATOR_MODULO, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%, BICOMC_OPERATOR_MODULO, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%, BICOMC_OPERATOR_MODULO, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%, BICOMC_OPERATOR_MODULO, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^, BICOMC_OPERATOR_BITWISE_XOR, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^, BICOMC_OPERATOR_BITWISE_XOR, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^, BICOMC_OPERATOR_BITWISE_XOR, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^, BICOMC_OPERATOR_BITWISE_XOR, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&, BICOMC_OPERATOR_BITWISE_AND, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&, BICOMC_OPERATOR_BITWISE_AND, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&, BICOMC_OPERATOR_BITWISE_AND, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&, BICOMC_OPERATOR_BITWISE_AND, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|, BICOMC_OPERATOR_BITWISE_OR, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|, BICOMC_OPERATOR_BITWISE_OR, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|, BICOMC_OPERATOR_BITWISE_OR, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|, BICOMC_OPERATOR_BITWISE_OR, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator=, BICOMC_OPERATOR_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator=, BICOMC_OPERATOR_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator=, BICOMC_OPERATOR_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator=, BICOMC_OPERATOR_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<, BICOMC_OPERATOR_LESS, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<, BICOMC_OPERATOR_LESS, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<, BICOMC_OPERATOR_LESS, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<, BICOMC_OPERATOR_LESS, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>, BICOMC_OPERATOR_GREATER, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>, BICOMC_OPERATOR_GREATER, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>, BICOMC_OPERATOR_GREATER, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>, BICOMC_OPERATOR_GREATER, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+=, BICOMC_OPERATOR_PLUS_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+=, BICOMC_OPERATOR_PLUS_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+=, BICOMC_OPERATOR_PLUS_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PLUS_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator+=, BICOMC_OPERATOR_PLUS_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-=, BICOMC_OPERATOR_MINUS_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-=, BICOMC_OPERATOR_MINUS_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-=, BICOMC_OPERATOR_MINUS_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MINUS_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator-=, BICOMC_OPERATOR_MINUS_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*=, BICOMC_OPERATOR_PRODUCT_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*=, BICOMC_OPERATOR_PRODUCT_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*=, BICOMC_OPERATOR_PRODUCT_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_PRODUCT_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator*=, BICOMC_OPERATOR_PRODUCT_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/=, BICOMC_OPERATOR_DIVIDE_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/=, BICOMC_OPERATOR_DIVIDE_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/=, BICOMC_OPERATOR_DIVIDE_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_DIVIDE_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator/=, BICOMC_OPERATOR_DIVIDE_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%=, BICOMC_OPERATOR_MODULO_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%=, BICOMC_OPERATOR_MODULO_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%=, BICOMC_OPERATOR_MODULO_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_MODULO_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator%=, BICOMC_OPERATOR_MODULO_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^=, BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^=, BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^=, BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_XOR_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator^=, BICOMC_OPERATOR_BITWISE_XOR_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&=, BICOMC_OPERATOR_BITWISE_AND_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&=, BICOMC_OPERATOR_BITWISE_AND_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&=, BICOMC_OPERATOR_BITWISE_AND_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_AND_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&=, BICOMC_OPERATOR_BITWISE_AND_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|=, BICOMC_OPERATOR_BITWISE_OR_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|=, BICOMC_OPERATOR_BITWISE_OR_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|=, BICOMC_OPERATOR_BITWISE_OR_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_BITWISE_OR_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator|=, BICOMC_OPERATOR_BITWISE_OR_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<, BICOMC_OPERATOR_LEFT_SHIFT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<, BICOMC_OPERATOR_LEFT_SHIFT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<, BICOMC_OPERATOR_LEFT_SHIFT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<, BICOMC_OPERATOR_LEFT_SHIFT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>, BICOMC_OPERATOR_RIGHT_SHIFT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>, BICOMC_OPERATOR_RIGHT_SHIFT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>, BICOMC_OPERATOR_RIGHT_SHIFT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>, BICOMC_OPERATOR_RIGHT_SHIFT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<=, BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<=, BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<=, BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LEFT_SHIFT_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<<=, BICOMC_OPERATOR_LEFT_SHIFT_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_ASSIGN(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>=, BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_ASSIGN_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>=, BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_ASSIGN_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>=, BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_RIGHT_SHIFT_ASSIGN_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>>=, BICOMC_OPERATOR_RIGHT_SHIFT_ASSIGN, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_EQUAL(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator==, BICOMC_OPERATOR_EQUAL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_EQUAL_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator==, BICOMC_OPERATOR_EQUAL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_EQUAL_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator==, BICOMC_OPERATOR_EQUAL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_EQUAL_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator==, BICOMC_OPERATOR_EQUAL, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_NOT_EQUAL(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator!=, BICOMC_OPERATOR_NOT_EQUAL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_NOT_EQUAL_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator!=, BICOMC_OPERATOR_NOT_EQUAL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_NOT_EQUAL_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator!=, BICOMC_OPERATOR_NOT_EQUAL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_NOT_EQUAL_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator!=, BICOMC_OPERATOR_NOT_EQUAL, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_EQUAL(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<=, BICOMC_OPERATOR_LESS_EQUAL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_EQUAL_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<=, BICOMC_OPERATOR_LESS_EQUAL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_EQUAL_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<=, BICOMC_OPERATOR_LESS_EQUAL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LESS_EQUAL_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator<=, BICOMC_OPERATOR_LESS_EQUAL, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_EQUAL(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>=, BICOMC_OPERATOR_GREATER_EQUAL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_EQUAL_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>=, BICOMC_OPERATOR_GREATER_EQUAL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_EQUAL_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>=, BICOMC_OPERATOR_GREATER_EQUAL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_GREATER_EQUAL_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator>=, BICOMC_OPERATOR_GREATER_EQUAL, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_AND(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&&, BICOMC_OPERATOR_LOGICAL_AND, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_AND_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&&, BICOMC_OPERATOR_LOGICAL_AND, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_AND_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&&, BICOMC_OPERATOR_LOGICAL_AND, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_AND_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator&&, BICOMC_OPERATOR_LOGICAL_AND, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_OR(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator||, BICOMC_OPERATOR_LOGICAL_OR, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_OR_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator||, BICOMC_OPERATOR_LOGICAL_OR, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_OR_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator||, BICOMC_OPERATOR_LOGICAL_OR, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_LOGICAL_OR_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator||, BICOMC_OPERATOR_LOGICAL_OR, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_COMMA(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(BICOMC_MACRO_PASS((operator,)), BICOMC_OPERATOR_COMMA, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_COMMA_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(BICOMC_MACRO_PASS((operator,)), BICOMC_OPERATOR_COMMA, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_COMMA_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(BICOMC_MACRO_PASS((operator,)), BICOMC_OPERATOR_COMMA, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_COMMA_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(BICOMC_MACRO_PASS((operator,), BICOMC_OPERATOR_COMMA, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_INCREMENT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator++, BICOMC_OPERATOR_POSTFIX_INCREMENT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_INCREMENT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator++, BICOMC_OPERATOR_POSTFIX_INCREMENT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_INCREMENT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator++, BICOMC_OPERATOR_POSTFIX_INCREMENT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_INCREMENT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator++, BICOMC_OPERATOR_POSTFIX_INCREMENT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_DECREMENT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator--, BICOMC_OPERATOR_POSTFIX_DECREMENT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_DECREMENT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator--, BICOMC_OPERATOR_POSTFIX_DECREMENT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_DECREMENT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator--, BICOMC_OPERATOR_POSTFIX_DECREMENT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POSTFIX_DECREMENT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator--, BICOMC_OPERATOR_POSTFIX_DECREMENT, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POINTER_TO_MEMBER(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator->*, BICOMC_OPERATOR_POINTER_TO_MEMBER, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POINTER_TO_MEMBER_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator->*, BICOMC_OPERATOR_POINTER_TO_MEMBER, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POINTER_TO_MEMBER_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator->*, BICOMC_OPERATOR_POINTER_TO_MEMBER, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_POINTER_TO_MEMBER_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator->*, BICOMC_OPERATOR_POINTER_TO_MEMBER, const volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_SUBSCRIPT(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator[], BICOMC_OPERATOR_SUBSCRIPT, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_SUBSCRIPT_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator[], BICOMC_OPERATOR_SUBSCRIPT, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_SUBSCRIPT_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator[], BICOMC_OPERATOR_SUBSCRIPT, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_SUBSCRIPT_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator[], BICOMC_OPERATOR_SUBSCRIPT, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_1(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_1_C(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_1_V(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_1_CV(METHOD_TYPE) BICOMC_DECL_METHOD_1_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_2(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_2_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_2_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_2_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_2(METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_2_C(METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_2_V(METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_2_CV(METHOD_TYPE) BICOMC_DECL_METHOD_2_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_3(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_3_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_3_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_3_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_3(METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_3_C(METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_3_V(METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_3_CV(METHOD_TYPE) BICOMC_DECL_METHOD_3_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_4(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_4_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_4_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_4_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_4(METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_4_C(METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_4_V(METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_4_CV(METHOD_TYPE) BICOMC_DECL_METHOD_4_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_5_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_5_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_5_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_5(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_5_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_5_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_5_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_5(METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_5_C(METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_5_V(METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_5_CV(METHOD_TYPE) BICOMC_DECL_METHOD_5_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_6_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_6_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_6_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_6(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_6_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_6_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_6_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_6(METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_6_C(METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_6_V(METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_6_CV(METHOD_TYPE) BICOMC_DECL_METHOD_6_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_7_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_7_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_7_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_7(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_7_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_7_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_7_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_7(METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_7_C(METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_7_V(METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_7_CV(METHOD_TYPE) BICOMC_DECL_METHOD_7_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_8_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_8_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_8_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_8(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_8_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_8_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_8_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_8(METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_8_C(METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_8_V(METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_8_CV(METHOD_TYPE) BICOMC_DECL_METHOD_8_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_9_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_9_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_9_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_9(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_9_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_9_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_9_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_9(METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_9_C(METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_9_V(METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_9_CV(METHOD_TYPE) BICOMC_DECL_METHOD_9_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_10_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_10_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_10_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_10(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_10_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_10_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_10_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_10(METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_10_C(METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_10_V(METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_10_CV(METHOD_TYPE) BICOMC_DECL_METHOD_10_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_11_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	typedef bcc::tuple_element<10, Params>::type P11; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_11_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
		, bcc::detail::Compatibility<bcc::tuple_element<10, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p11 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_11_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_11(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_11_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_11_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_11_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_11(METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_11_C(METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_11_V(METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_11_CV(METHOD_TYPE) BICOMC_DECL_METHOD_11_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_12_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	typedef bcc::tuple_element<10, Params>::type P11; \
	typedef bcc::tuple_element<11, Params>::type P12; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_12_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
		, bcc::detail::Compatibility<bcc::tuple_element<10, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p11 \
		, bcc::detail::Compatibility<bcc::tuple_element<11, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p12 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_12_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_12(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_12_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_12_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_12_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_12(METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_12_C(METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_12_V(METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_12_CV(METHOD_TYPE) BICOMC_DECL_METHOD_12_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_13_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	typedef bcc::tuple_element<10, Params>::type P11; \
	typedef bcc::tuple_element<11, Params>::type P12; \
	typedef bcc::tuple_element<12, Params>::type P13; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_13_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
		, bcc::detail::Compatibility<bcc::tuple_element<10, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p11 \
		, bcc::detail::Compatibility<bcc::tuple_element<11, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p12 \
		, bcc::detail::Compatibility<bcc::tuple_element<12, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p13 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_13_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_13(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_13_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_13_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_13_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_13(METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_13_C(METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_13_V(METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_13_CV(METHOD_TYPE) BICOMC_DECL_METHOD_13_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_14_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	typedef bcc::tuple_element<10, Params>::type P11; \
	typedef bcc::tuple_element<11, Params>::type P12; \
	typedef bcc::tuple_element<12, Params>::type P13; \
	typedef bcc::tuple_element<13, Params>::type P14; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_14_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
		, bcc::detail::Compatibility<bcc::tuple_element<10, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p11 \
		, bcc::detail::Compatibility<bcc::tuple_element<11, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p12 \
		, bcc::detail::Compatibility<bcc::tuple_element<12, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p13 \
		, bcc::detail::Compatibility<bcc::tuple_element<13, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p14 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_14_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_14(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_14_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_14_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_14_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_14(METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_14_C(METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_14_V(METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_14_CV(METHOD_TYPE) BICOMC_DECL_METHOD_14_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_15_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	typedef bcc::tuple_element<4, Params>::type P5; \
	typedef bcc::tuple_element<5, Params>::type P6; \
	typedef bcc::tuple_element<6, Params>::type P7; \
	typedef bcc::tuple_element<7, Params>::type P8; \
	typedef bcc::tuple_element<8, Params>::type P9; \
	typedef bcc::tuple_element<9, Params>::type P10; \
	typedef bcc::tuple_element<10, Params>::type P11; \
	typedef bcc::tuple_element<11, Params>::type P12; \
	typedef bcc::tuple_element<12, Params>::type P13; \
	typedef bcc::tuple_element<13, Params>::type P14; \
	typedef bcc::tuple_element<14, Params>::type P15; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_15_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(this, &result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_BYNAME, METHOD_QUALIFIER, METHOD_TYPE) \
	BICOMC_STATIC_ASSERT((bcc::is_function<METHOD_TYPE>::value), "'" #METHOD_TYPE "' must be function type.", function_type_check_ ## METHOD_BYNAME); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER); \
	BICOMC_STATIC_ASSERT(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_BYNAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared.", function_redecl_check_ ## METHOD_BYNAME); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_BYNAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
		, bcc::detail::Compatibility<bcc::tuple_element<5, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p6 \
		, bcc::detail::Compatibility<bcc::tuple_element<6, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p7 \
		, bcc::detail::Compatibility<bcc::tuple_element<7, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p8 \
		, bcc::detail::Compatibility<bcc::tuple_element<8, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p9 \
		, bcc::detail::Compatibility<bcc::tuple_element<9, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p10 \
		, bcc::detail::Compatibility<bcc::tuple_element<10, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p11 \
		, bcc::detail::Compatibility<bcc::tuple_element<11, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p12 \
		, bcc::detail::Compatibility<bcc::tuple_element<12, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p13 \
		, bcc::detail::Compatibility<bcc::tuple_element<13, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p14 \
		, bcc::detail::Compatibility<bcc::tuple_element<14, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p15 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_BYNAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_15_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_15(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_15_C(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_15_V(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_15_CV(METHOD_NAME, METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL_15(METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_15_C(METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_15_V(METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_15_CV(METHOD_TYPE) BICOMC_DECL_METHOD_15_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#define BICOMC_DECL_METHOD(METHOD_NAME, METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(METHOD_NAME, METHOD_NAME, , METHOD_TYPE)
#define BICOMC_DECL_METHOD_C(METHOD_NAME, METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(METHOD_NAME, METHOD_NAME, const, METHOD_TYPE)
#define BICOMC_DECL_METHOD_V(METHOD_NAME, METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(METHOD_NAME, METHOD_NAME, volatile, METHOD_TYPE)
#define BICOMC_DECL_METHOD_CV(METHOD_NAME, METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(METHOD_NAME, METHOD_NAME, const volatile, METHOD_TYPE)

#define BICOMC_DECL_OPERATOR_CALL(METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, , METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_C(METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_V(METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, volatile, METHOD_TYPE)
#define BICOMC_DECL_OPERATOR_CALL_CV(METHOD_TYPE, PARAMETER_SIZE) BICOMC_DECL_METHOD_##PARAMETER_SIZE##_QUALIFIER(operator(), BICOMC_OPERATOR_CALL, const volatile, METHOD_TYPE)

#endif // !def BICOMC_DETAIL_OBJECT_FORWARD_H__
