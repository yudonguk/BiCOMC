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

#include "compatibility.h"
#include "hash.h"
#include "method_traits.h"
#include "object_helper.h"
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

	struct DefaultCallHelper
	{
		template<typename T>
		struct Helper;

		template<typename T>
		static typename MethodTypeDeducer<void(), bcc::Object, true, true>::helper* destroy(T const volatile& impl);

		template<typename HelpInterface, typename T>
		static typename MethodTypeDeducer<bcc::Object*(), bcc::Object, true, false>::helper* clone(T const& impl);
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
	struct InterfaceTableHolder
	{
		static size_t const HEADER_SIZE = ObjectHelper::VFTABLE_HEADER_SIZE;

		typedef typename bcc::detail::FunctionTypes<Interface>::type FunctionTypes;
		static size_t const depth = bcc::tuple_size<FunctionTypes>::value - 1;

		typedef typename bcc::detail::RawTable<FunctionTypes>::type RawTable;

		template<typename T>
		InterfaceTableHolder(T& object)
			: name(bcc::detail::Signature<typename bcc::remove_cv<T>::type>::to_utf8())
		{
			InterfaceInfoDeducer<FunctionTypes>::template init<Interface>(info, name.c_str());

			TableCopyHelper<RawTable, 0, depth>::copy(table, vftable.data() + HEADER_SIZE, object);
			bcc::get<0>(bcc::get<depth>(table)) = info.data();
			vftable[0] = reinterpret_cast<void*>(depth); // depth
			vftable[1] = 0; // first
			vftable[2] = 0; // next
			vftable[depth + HEADER_SIZE] = &bcc::get<depth>(table); // table
		}

		std::string const name;
		typename InterfaceInfoDeducer<FunctionTypes>::raw_type info;
		RawTable table;
		bcc::array<void*, bcc::tuple_size<RawTable>::value + HEADER_SIZE> vftable;
	};

	template<typename Interfaces, size_t size>
	struct DeduplicationHelper
	{
		template<size_t index = 0, size_t end = size, typename Dummy = void>
		struct Helper
		{
			template<typename Holders>
			static void remove(Holders& holders)
			{
				typedef typename bcc::tuple_element<index - 1, Interfaces>::type Prev;
				typedef typename bcc::tuple_element<size - 1, Interfaces>::type Now;

				typename bcc::tuple_element<index - 1, Holders>::type& prevHolder = bcc::get<index - 1>(holders);
				typename bcc::tuple_element<size - 1, Holders>::type& nowHolder = bcc::get<size - 1>(holders);

				Helper2<Now, Prev>::remove(bcc::get<1>(nowHolder), bcc::get<1>(prevHolder));

				Helper<index + 1, end>::remove(holders);
			}
		};

		template<size_t end, typename Dummy>
		struct Helper<0, end, Dummy>
		{
			template<typename Holders>
			static void remove(Holders& holders)
			{
				Helper<1, end>::remove(holders);
			}
		};

		template<typename Dummy>
		struct Helper<0, 0, Dummy>
		{
			template<typename Holders>
			static void remove(Holders& holders) {}
		};

		template<size_t end, typename Dummy>
		struct Helper<end, end, Dummy>
		{
			template<typename Holders>
			static void remove(Holders& holders) {}
		};

		template<typename Interface, typename Basis, typename Dummy = void>
		struct Helper2
		{
			template<typename T, typename Dummy2 = void>
			struct Helper
			{
				template<typename Vftable, typename BasisVftable>
				static void remove(Vftable& vftable, BasisVftable& basis)
				{
					Helper<typename LazyBase<T>::type>::remove(vftable, basis);

					if (bcc::is_same<Interface, T>::value)
					{
						bcc::get<InheritanceDepth<Interface>::value + ObjectHelper::VFTABLE_HEADER_SIZE>(vftable)
							= bcc::get<InheritanceDepth<T>::value + ObjectHelper::VFTABLE_HEADER_SIZE>(basis);
					}
				}
			};

			template<typename Dummy2>
			struct Helper<void, Dummy2>
			{
				template<typename Vftable, typename BasisVftable>
				static void remove(Vftable& vftable, BasisVftable& basis) {}
			};


			template<typename Vftable, typename BasisVftable>
			static void remove(Vftable& vftable, BasisVftable& basis)
			{
				typedef typename LazyBase<Interface>::type Base;
				Helper2<Base, Basis>::remove(vftable, basis);
				Helper<Basis>::remove(vftable, basis);
			}
		};

		template<typename Interface, typename Dummy>
		struct Helper2<Interface, void, Dummy>
		{
			template<typename Vftable, typename BasisVftable>
			static void remove(Vftable& vftable, BasisVftable& basis) {}
		};

		template<typename Basis, typename Dummy>
		struct Helper2<void, Basis, Dummy>
		{
			template<typename Vftable, typename BasisVftable>
			static void remove(Vftable& vftable, BasisVftable& basis) {}
		};

		template<typename Dummy>
		struct Helper2<void, void, Dummy>
		{
			template<typename Vftable, typename BasisVftable>
			static void remove(Vftable& vftable, BasisVftable& basis) {}
		};

		template<typename Holders>
		static void remove(Holders& holders)
		{
			Helper<>::remove(holders);
		}
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

		template<typename Target, size_t size = bcc::tuple_size<Interfaces>::value>
		struct CompatibilityTester
		{
			typedef typename bcc::tuple_element<size - 1, Interfaces>::type Interface;

			static bool test()
			{
				return CompatibilityTester<Target, size - 1>::test()
					|| reinterpret_cast<Target*>(1) == static_cast<Target*>(reinterpret_cast<Interface*>(1));
			}
		};

		template<typename Target>
		struct CompatibilityTester<Target, 0>
		{
			static bool test() { return false; }
		};

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
			static void init(H& holders, T& object, bcc::Object const& firstObject)
			{
				Helper<size - 1>::init(holders, object, firstObject);

				Interface& impl = static_cast<Interface&>(object);
				Holder& holder = bcc::get<size - 1>(holders);
				RawTable& table = bcc::get<0>(holder);
				Vftable& vftable = bcc::get<1>(holder);

				TableCopyHelper<RawTable>::copy(table, vftable.data() + HEADER_SIZE, impl);
				vftable[0] = reinterpret_cast<void*>(bcc::tuple_size<FunctionTypes>::value - 1); // depth
				vftable[1] = reinterpret_cast<void*>(first(object, firstObject)); // first
				vftable[2] = reinterpret_cast<void*>(next(object)); // next

				DeduplicationHelper<Interfaces, size>::remove(holders);
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

			template<typename T>
			static bcc::Object const* first(T& object)
			{
				bcc::Object const* p = Helper<size - 1>::first(object);
				bcc::Object const* p2 = &static_cast<bcc::Object const&>(static_cast<Interface const&>(object));
				return p < p2 ? p : p2;
			}

			template<typename T>
			static bcc::intptr_t first(T& object, bcc::Object const& firstObject)
			{
				return reinterpret_cast<char const*>(&firstObject)
					- reinterpret_cast<char const*>(&static_cast<bcc::Object const&>(static_cast<Interface const&>(object)));
			}
		};

		template<typename Dummy>
		struct Helper<0, Dummy>
		{
			typedef bcc::tuple<> type;

			template<typename H, typename T>
			static void init(H& holder, T& object, bcc::Object const& firstObject) {}

			template<typename H, typename T>
			static void setVftable(T& object, H& holders) {}

			template<typename T>
			static bcc::Object const* first(T& object)
			{
				return reinterpret_cast<bcc::Object*>(static_cast<char*>(nullptr) - 1);
			}
		};

		template<typename T>
		MultiTableHolder(T& object)
		{
			if (!CompatibilityTester<T>::test())
				throw std::runtime_error("'T' is not binary compatible. Please put all interfaces into BICOMC_OVERRIDE() or arrange base type order which is interface first.");
			Helper<>::init(holder, object, *Helper<>::first(object));
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

			bcc::detail::Sha1 sha1;
			sha1.calculate(data.c_str(), data.c_str() + data.size());

			bcc::array<bcc::uint32_t, 5> hash = sha1.get();
			return *reinterpret_cast<bcc::uint64_t*>(hash.data());
		}

		static bcc::uint64_t subhash()
		{
			std::string data;
			SubhashHelper<>::list(data);

			bcc::detail::Sha1 sha1;
			sha1.calculate(data.c_str(), data.c_str() + data.size());

			bcc::array<bcc::uint32_t, 5> hash = sha1.get();
			return *reinterpret_cast<bcc::uint64_t*>(hash.data());
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

	inline bool is_multiple(bcc::Object const& object)
	{
		return bcc::detail::ObjectHelper::isMultiple(object);
	}

	inline bool is_multiple(bcc::Object const volatile& object)
	{
		return bcc::detail::ObjectHelper::isMultiple(object);
	}

	template<typename Target>
	bool is_compatible(bcc::Object const& object)
	{
		static_assert(bcc::is_interface<Target>::value, "'Target' type must be interface.");
		class Temp : public bcc::remove_cv<Target>::type {} target;
		return bcc::detail::ObjectHelper::isCompatible(object, target);
	}

	template<typename Target>
	bool is_compatible(bcc::Object const volatile& object)
	{
		static_assert(bcc::is_interface<Target>::value, "'Target' type must be interface.");
		class Temp : public bcc::remove_cv<Target>::type {} target;
		return bcc::detail::ObjectHelper::isCompatible(object, target);
	}

	template<typename T, typename U>
	T bicomc_cast(U& object);

	template<typename T, typename U>
	T bicomc_cast(U* object);
} // namespace bcc

using bcc::bicomc_cast;

#define BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME) \
protected: \
	template<typename> friend struct bcc::detail::InheritanceDepth; \
	static size_t const BICOMC_INHERITANCE_DEPTH__ = bcc::detail::InheritanceDepth<INTERFACE_NAME>::value; \
private: \
	template<typename> friend struct bcc::detail::LazyBase; \
	typedef INTERFACE_NAME BiCOMC_My__; \
	\
	template<size_t index, typename Dummy = void> \
	struct BiCOMC_Type_Enumerator__; \
	template<typename Dummy> \
	struct BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, __COUNTER__>::value, Dummy> \
	{ \
		typedef void* type; \
		typedef bcc::tuple<type> list; \
	}

#define BICOMC_INTERFACE_DERIVED(INTERFACE_NAME, BASE_NAME) \
class INTERFACE_NAME : public BASE_NAME \
{ \
private: \
	friend class bcc::detail::ObjectHelper; \
	template<typename> friend struct bcc::detail::is_interface_impl; \
	static_assert(bcc::is_interface<BASE_NAME>::value, "'" #BASE_NAME "' must be interface."); \
	\
	typedef BASE_NAME BiCOMC_Base__; \
	BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME);

#define BICOMC_INTERFACE(INTERFACE_NAME, ...) \
static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_INTERFACE() has one or two parameters."); \
class INTERFACE_NAME : public bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type \
{ \
private: \
	friend class bcc::detail::ObjectHelper; \
	template<typename> friend struct bcc::detail::is_interface_impl; \
	\
	typedef bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type BiCOMC_Base__; \
	static_assert(bcc::is_interface<BiCOMC_Base__>::value, "Base type must be interface."); \
	BICOMC_INTERFACE_BASE_CODE(INTERFACE_NAME);

#define BICOMC_INTERFACE_FUNCTION_TABLE(INTERFACE_NAME) \
private: \
	template<typename Owner, typename Dummy = void> \
	struct BiCOMC_Type_Decider__ \
	{ \
		typedef BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, __COUNTER__>::value - 1>::list list; \
		typedef typename bcc::detail::InterfaceInfoDeducer<bcc::tuple<list> >::type info_type; \
		typedef typename bcc::TupleCat< \
			typename Owner::BiCOMC_Base__::BiCOMC_Function_Types__ \
			, typename bcc::tuple_replace<list, 0, info_type>::type \
		>::type type; \
	}; \
	template<typename Dummy> \
	struct BiCOMC_Type_Decider__<bcc::Object, Dummy> \
	{ \
		typedef BiCOMC_Type_Enumerator__<bcc::detail::EnumeratorSize<BiCOMC_Type_Enumerator__, __COUNTER__>::value - 1>::list list; \
		typedef typename bcc::detail::InterfaceInfoDeducer<bcc::tuple<list> >::type info_type; \
		typedef bcc::tuple<typename bcc::tuple_replace<list, 0, info_type>::type> type; \
	}; \
protected: \
	template<typename> friend struct bcc::detail::FunctionTypes; \
	typedef BiCOMC_Type_Decider__<INTERFACE_NAME>::type BiCOMC_Function_Types__;

#define BICOMC_INTERFACE_END(INTERFACE_NAME) \
protected: \
	BICOMC_INTERFACE_FUNCTION_TABLE(INTERFACE_NAME); \
protected: \
	INTERFACE_NAME() \
		: BiCOMC_Base__() \
	{ \
		static bcc::detail::InterfaceTableHolder<INTERFACE_NAME> holder(*this); \
		bcc::detail::ObjectHelper::setTable(*this, holder.vftable.data()); \
	} \
	~INTERFACE_NAME() {} \
private: \
	INTERFACE_NAME(BiCOMC_My__ const& rhs) BICOMC_DELETE; \
	BiCOMC_My__& operator=(BiCOMC_My__ const& rhs) BICOMC_DELETE; \
}; \
template<> struct BICOMC_SIGNATURE_DEFAULT_NAME<INTERFACE_NAME> { static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(INTERFACE_NAME); } };

#define BICOMC_METHOD_TYPE_NAME(METHOD_NAME) \
	BiCOMC_ ## METHOD_NAME ## _Type__

#define BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
protected: \
	template<typename Function, bool isConst, bool isVolatile, typename Dummy> struct BICOMC_METHOD_TYPE_NAME(METHOD_NAME); \
	template<typename Dummy> \
	struct BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, Dummy> \
	{ \
		typedef BiCOMC_My__ METHOD_QUALIFIER owner; \
		typedef bcc::detail::MethodTypeDeducer<METHOD_TYPE, owner> deducer; \
		typedef typename deducer::trait trait; \
		\
		static size_t const depth = owner::BICOMC_INHERITANCE_DEPTH__; \
		static size_t const index = bcc::detail::EnumeratorSize<owner::BiCOMC_Type_Enumerator__, __COUNTER__>::value; \
		\
		template<typename Impl> \
		static typename deducer::helper*& function(Impl& impl) \
		{ \
			return bcc::detail::ObjectHelper::function<index, depth, owner::BiCOMC_Function_Types__>(static_cast<owner&>(impl)); \
		} \
		template<typename Base, typename Impl> \
		static void overrideMethod(Impl& impl) \
		{ \
			function(static_cast<Base&>(impl)) = &bcc::detail::MethodCallHelper<typename deducer::template change_owner<Impl>::member, &Impl::METHOD_NAME>::template call<owner>; \
		} \
		static char const* name() \
		{ \
			static std::string const name_(bcc::detail::StringUtil::convertToUtf8(BICOMC_WSTRINGIZER(METHOD_NAME))); \
			return name_.c_str(); \
		} \
		static char const* signature() \
		{ \
			static std::string const signature_( \
				std::string(name()) \
				.append(bcc::detail::StringUtil::convertToUtf8(L"=")) \
				.append(bcc::detail::StringUtil::convertToUtf8(bcc::detail::Signature<deducer>::to_wstring())) \
			); \
			return signature_.c_str(); \
		} \
	}; \
private: \
	template<typename Dummy> \
	struct BiCOMC_Type_Enumerator__<BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void>::index, Dummy> \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MethodType; \
		typedef MethodType type; \
		typedef typename bcc::TupleCat<typename BiCOMC_Type_Enumerator__<MethodType::index - 1>::list, type>::type list; \
	};

#define BICOMC_METHOD_TYPE_CHECKER(INTERFACE_NAME) \
	BiCOMC_ ## INTERFACE_NAME ## _Type_Checker__

#define BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
private: \
	template<typename T, typename Function, bool isConst, bool isVolatile> struct BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME); \
	template<typename T> \
	struct BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<T, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value> \
	{ \
		typedef bcc::int8_t TrueType; \
		typedef bcc::int16_t FalseType; \
		template<template<typename, bool, bool, typename> class U> \
		struct Helper { typedef U<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> type; }; \
		template<typename U> static TrueType test(typename Helper<U::template BICOMC_METHOD_TYPE_NAME(METHOD_NAME)>::type::owner*); \
		template<typename U> static FalseType test(...); \
		static bool const value = sizeof(test<T>(0)) == sizeof(TrueType); \
	};

#define BICOMC_OVER_METHOD(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, false, void>::overrideMethod<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type>(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_METHOD() has two or three parameters."); \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type.");

#define BICOMC_OVER_METHOD_C(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, false, void>::overrideMethod<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type>(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_METHOD_C() has two or three parameters."); \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type.");

#define BICOMC_OVER_METHOD_V(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, true, void>::overrideMethod<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type>(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_METHOD_V() has two or three parameters."); \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type.");

#define BICOMC_OVER_METHOD_CV(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, true, void>::overrideMethod<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type>(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_METHOD_CV() has two or three parameters."); \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type.");

#define BICOMC_METHOD(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, false, void>::function(static_cast<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type&>(*this))

#define BICOMC_METHOD_C(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, false, void>::function(static_cast<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type const&>(*this))

#define BICOMC_METHOD_V(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, false, true, void>::function(static_cast<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type volatile&>(*this))

#define BICOMC_METHOD_CV(METHOD_NAME, METHOD_TYPE, ...) \
	BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, true, true, void>::function(static_cast<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type const volatile&>(*this))

#define BICOMC_OVER_DESTROY(...) \
	BICOMC_METHOD_CV(destroy, void(), __VA_ARGS__) = bcc::detail::DefaultCallHelper::destroy(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_DESTROY() has one or no parameter.");

#define BICOMC_OVER_CLONE(...) \
	BICOMC_METHOD_C(clone, bcc::Object*(), __VA_ARGS__) = bcc::detail::DefaultCallHelper::clone<bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type>(*this); \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value <= 1, "BICOMC_OVER_CLONE() has one or no parameter.");

#define BICOMC_OVERRIDE(...) \
protected: \
	static_assert(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value != 0, "BICOMC_OVERRIDE() has one or more paramters."); \
	friend struct bcc::detail::DefaultCallHelper; \
	friend struct bcc::detail::ObjectCaster; \
	typedef bcc::conditional<(bcc::tuple_size<bcc::tuple<__VA_ARGS__> >::value > 1), bcc::true_type, bcc::false_type>::type BiCOMC_Multiple_Inheritnace_Checker; \
private: \
	inline bool BiCOMC_Override_Method_Helper__() \
	{ \
		static bcc::detail::MultiTableHolder<bcc::tuple<__VA_ARGS__> > holders(*this); \
		static bool isInitialized = false; \
		holders.setVftable(*this); \
		if (!isInitialized) \
		{ \
			BICOMC_OVER_DESTROY(bcc::tuple_element<0, bcc::TupleCat<bcc::tuple<__VA_ARGS__>, bcc::Object>::type>::type);

#define BICOMC_OVERRIDE_END() \
			isInitialized = true; \
		} \
		return isInitialized; \
	} \
	bcc::detail::OverrideInitHolder BiCOMC_override_init_holder; \
public:

#define BICOMC_OVERRIDE_INIT() BiCOMC_override_init_holder(BiCOMC_Override_Method_Helper__())

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME() METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_0_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_0(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_0_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_0_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_0_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_0_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_1_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_1(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_1_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_1_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_1_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_1_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_2_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_2(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_2_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_2_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_2_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_2_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_3_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_3(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_3_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_3_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_3_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_3_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

//////////////////////////////////////////////////////////////////////////

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	typedef MT::trait::params Params; \
	typedef bcc::tuple_element<0, Params>::type P1; \
	typedef bcc::tuple_element<1, Params>::type P2; \
	typedef bcc::tuple_element<2, Params>::type P3; \
	typedef bcc::tuple_element<3, Params>::type P4; \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_4_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_4(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_4_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_4_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_4_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_4_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_5_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
public: \
	bcc::detail::Compatibility<bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::ret>::type METHOD_NAME( \
		bcc::detail::Compatibility<bcc::tuple_element<0, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p1 \
		, bcc::detail::Compatibility<bcc::tuple_element<1, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p2 \
		, bcc::detail::Compatibility<bcc::tuple_element<2, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p3 \
		, bcc::detail::Compatibility<bcc::tuple_element<3, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p4 \
		, bcc::detail::Compatibility<bcc::tuple_element<4, bcc::detail::MethodTypeTrait<METHOD_TYPE, BiCOMC_My__>::params>::type>::type p5 \
	) METHOD_QUALIFIER \
	{ \
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_5_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_5(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_5_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_5_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_5_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_5_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_6_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_6_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_6(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_6_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_6_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_6_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_6_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_7_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_7_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_7(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_7_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_7_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_7_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_7_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_8_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_8_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_8(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_8_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_8_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_8_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_8_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_9_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_9_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_9(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_9_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_9_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_9_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_9_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_10_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_10_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_10(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_10_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_10_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_10_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_10_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_11_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_11_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_11(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_11_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_11_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_11_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_11_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_12_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_12_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_12(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_12_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_12_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_12_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_12_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_13_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_13_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_13(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_13_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_13_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_13_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_13_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_14_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_14_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_14(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_14_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_14_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_14_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_14_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

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
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15))) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(std::move(result));
#else // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#define BICOMC_DECL_METHOD_15_QUALIFIER_METHOD_CALL \
	RH::mediator result; \
	if (bcc::detail::ErrorDetail* e = MT::function(*this)(*this, result, p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15)) \
		throw bcc::ErrorCode(e); \
	return RH::toReturn(result);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#define BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, METHOD_QUALIFIER, METHOD_TYPE) \
	static_assert(bcc::is_function<METHOD_TYPE>::value, "'" #METHOD_TYPE "' must be function type."); \
private: \
	BICOMC_METHOD_TYPE_CHECKER_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER); \
	static_assert(!(BICOMC_METHOD_TYPE_CHECKER(METHOD_NAME)<bcc::detail::LazyBase<BiCOMC_My__>::type, METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value>::value), "'" #METHOD_NAME "' was already declared."); \
protected: \
	BICOMC_METHOD_TYPE_DEF(METHOD_NAME, METHOD_TYPE, METHOD_QUALIFIER) \
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
		typedef BICOMC_METHOD_TYPE_NAME(METHOD_NAME)<METHOD_TYPE, bcc::is_const<int METHOD_QUALIFIER>::value, bcc::is_volatile<int METHOD_QUALIFIER>::value, void> MT; \
		typedef bcc::detail::ReturnHelper<MT::trait::ret> RH; \
		BICOMC_DECL_METHOD_15_QUALIFIER_METHOD_CALL \
	}

#define BICOMC_DECL_METHOD_15(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, , METHOD_TYPE)

#define BICOMC_DECL_METHOD_15_C(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, const, METHOD_TYPE)

#define BICOMC_DECL_METHOD_15_V(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, volatile, METHOD_TYPE)

#define BICOMC_DECL_METHOD_15_CV(METHOD_NAME, METHOD_TYPE) \
	BICOMC_DECL_METHOD_15_QUALIFIER(METHOD_NAME, const volatile, METHOD_TYPE)

#endif // !def BICOMC_DETAIL_OBJECT_FORWARD_H__
