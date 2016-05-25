#ifndef BICOMC_TUPLE_H__
#define BICOMC_TUPLE_H__

#include "detail/config.h"

#include <utility>

#include "stdint.h"
#include "type_traits.h"

namespace bcc
{
	template<typename TupleType>
	struct tuple_size;

	template<size_t index, typename TupleType>
	struct tuple_element;

#if BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename... Types>
	class tuple;

	template<typename T1, typename... Types>
	class tuple<T1, Types...>
	{
	public:
		typedef T1 Value;
		typedef tuple<Types...> Next;

		template<typename>
		friend struct tuple_size;

		template<size_t, typename>
		friend struct tuple_element;

	public:
		tuple()
			: value(), next()
		{}

		~tuple()
		{}

	private:
		Value value;
		Next next;
	};

	template<>
	class tuple<>
	{};

	template<typename... Types>
	struct tuple_size<tuple<Types...> >
		: public bcc::integral_constant<size_t, sizeof...(Types)>
	{};

	template<size_t index, typename Head, typename... Tails>
	struct tuple_element<index, tuple<Head, Tails...> >
	{
		typedef tuple<Head, Tails...> TupleType;
		typedef typename tuple_element<index - 1, tuple<Tails...> >::type type;

		static BICOMC_CONSTEXPR type& get(TupleType& tuple) BICOMC_NOEXCEPT
		{
			return tuple_element<index - 1, typename TupleType::Next>::get(tuple.next);
		}

		static BICOMC_CONSTEXPR type const& get(TupleType const& tuple) BICOMC_NOEXCEPT
		{
			return tuple_element<index - 1, typename TupleType::Next>::get(tuple.next);
		}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static BICOMC_CONSTEXPR type&& get(TupleType&& tuple) BICOMC_NOEXCEPT
		{
			return std::move(tuple_element<index - 1, typename TupleType::Next>::get(tuple.next));
		}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	};

	template<typename Head, typename... Tails>
	struct tuple_element<0, tuple<Head, Tails...> >
	{
		typedef tuple<Head, Tails...> TupleType;
		typedef Head type;

		static BICOMC_CONSTEXPR type& get(TupleType& tuple) BICOMC_NOEXCEPT
		{
			return tuple.value;
		}

		static BICOMC_CONSTEXPR type const& get(TupleType const& tuple) BICOMC_NOEXCEPT
		{
			return tuple.value;
		}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static BICOMC_CONSTEXPR type&& get(TupleType&& tuple) BICOMC_NOEXCEPT
		{
			return std::move(tuple.value);
		}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	};

	template<size_t index, typename... Types>
	BICOMC_CONSTEXPR typename tuple_element<index, tuple<Types...> >::type& get(tuple<Types...>& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<Types...> >::get(tuple);
	}

	template<size_t index, typename... Types>
	BICOMC_CONSTEXPR typename tuple_element<index, tuple<Types...> >::type const& get(tuple<Types...> const& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<Types...> >::get(tuple);
	}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<size_t index, typename... Types>
	BICOMC_CONSTEXPR typename tuple_element<index, tuple<Types...> >::type&& get(tuple<Types...>&& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<Types...> >::get(std::move(tuple));
	}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#else
	template<typename T1 = void, typename T2 = void, typename T3 = void
		, typename T4 = void, typename T5 = void, typename T6 = void
		, typename T7 = void, typename T8 = void, typename T9 = void
		, typename T10 = void, typename T11 = void, typename T12 = void
		, typename T13 = void, typename T14 = void, typename T15 = void
		, typename T16 = void, typename T17 = void, typename T18 = void
		, typename T19 = void, typename T20 = void, typename T21 = void
		, typename T22 = void, typename T23 = void, typename T24 = void
		, typename T25 = void, typename T26 = void, typename T27 = void
		, typename T28 = void, typename T29 = void, typename T30 = void>
	class tuple
	{
	public:
		typedef T1 Value;
		typedef tuple<T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14
			, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27
			, T28, T29, T30> Next;

		template<typename>
		friend struct tuple_size;

		template<size_t, typename>
		friend struct tuple_element;

		template<typename, typename>
		friend struct TupleCat;

	public:
		tuple()
			: value(), next()
		{}

		~tuple()
		{}

	private:
		Value value;
		Next next;
	};

	template<>
	class tuple<>
	{};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
		, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12
		, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18
		, typename T19, typename T20, typename T21, typename T22, typename T23, typename T24
		, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
	struct tuple_size<tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
		, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26
		, T27, T28, T29, T30> >
		: public bcc::integral_constant<size_t
		, tuple_size<typename tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
		, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26
		, T27, T28, T29, T30>::Next>::value + 1>
	{};

	template<size_t index, typename T1, typename T2, typename T3, typename T4, typename T5
		, typename T6, typename T7, typename T8, typename T9, typename T10, typename T11
		, typename T12, typename T13, typename T14, typename T15, typename T16, typename T17
		, typename T18, typename T19, typename T20, typename T21, typename T22, typename T23
		, typename T24, typename T25, typename T26, typename T27, typename T28, typename T29
		, typename T30>
	struct tuple_element<index, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
		, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26
		, T27, T28, T29, T30> >
	{
		typedef tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
			, T30> TupleType;
		typedef typename tuple_element<index - 1, typename TupleType::Next>::type type;

		static BICOMC_CONSTEXPR type& get(TupleType& tuple) BICOMC_NOEXCEPT
		{
			return tuple_element<index - 1, typename TupleType::Next>::get(tuple.next);
		}

		static BICOMC_CONSTEXPR type const& get(TupleType const& tuple) BICOMC_NOEXCEPT
		{
			return tuple_element<index - 1, typename TupleType::Next>::get(tuple.next);
		}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static BICOMC_CONSTEXPR type&& get(TupleType&& tuple) BICOMC_NOEXCEPT
		{
			return std::move(tuple_element<index - 1, typename TupleType::Next>::get(tuple.next));
		}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	};

	template<typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
		, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12
		, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18
		, typename T19, typename T20, typename T21, typename T22, typename T23, typename T24
		, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
	struct tuple_element<0, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12
		, T13, T14, T15, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26
		, T27, T28, T29, T30> >
	{
		typedef tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
			, T30> TupleType;
		typedef typename TupleType::Value type;

		static BICOMC_CONSTEXPR type& get(TupleType& tuple) BICOMC_NOEXCEPT
		{
			return tuple.value;
		}

		static BICOMC_CONSTEXPR type const& get(TupleType const& tuple) BICOMC_NOEXCEPT
		{
			return tuple.value;
		}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		static BICOMC_CONSTEXPR type&& get(TupleType&& tuple) BICOMC_NOEXCEPT
		{
			return std::move(tuple.value);
		}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	};

	template<size_t index
		, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
		, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12
		, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18
		, typename T19, typename T20, typename T21, typename T22, typename T23, typename T24
		, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
	BICOMC_CONSTEXPR typename tuple_element<index
		, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
			, T30>
		>::type& get(tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30>& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
				, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30> >::get(tuple);
	}

	template<size_t index
		, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
		, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12
		, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18
		, typename T19, typename T20, typename T21, typename T22, typename T23, typename T24
		, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
	BICOMC_CONSTEXPR typename tuple_element<index
		, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
			, T30>
		>::type const& get(tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30> const& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
				, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30> >::get(tuple);
	}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<size_t index
		, typename T1, typename T2, typename T3, typename T4, typename T5, typename T6
		, typename T7, typename T8, typename T9, typename T10, typename T11, typename T12
		, typename T13, typename T14, typename T15, typename T16, typename T17, typename T18
		, typename T19, typename T20, typename T21, typename T22, typename T23, typename T24
		, typename T25, typename T26, typename T27, typename T28, typename T29, typename T30>
	BICOMC_CONSTEXPR typename tuple_element<index
		, tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29
			, T30>
		>::type&& get(tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
			, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30>&& tuple) BICOMC_NOEXCEPT
	{
		return tuple_element<index, bcc::tuple<T1, T2, T3, T4, T5, T6, T7, T8, T9, T10, T11, T12, T13, T14, T15
				, T16, T17, T18, T19, T20, T21, T22, T23, T24, T25, T26, T27, T28, T29, T30> >::get(std::move(tuple));
	}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#endif // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER

	template<>
	struct tuple_size<tuple<> >
		: public bcc::integral_constant<size_t, 0>
	{};

	template<size_t index, typename TupleType>
	struct tuple_element<index, TupleType const>
	{
		typedef typename tuple_element<index, TupleType>::type const type;
	};

	template<size_t index, typename TupleType>
	struct tuple_element<index, TupleType volatile>
	{
		typedef typename tuple_element<index, TupleType>::type volatile type;
	};

	template<size_t index, typename TupleType>
	struct tuple_element<index, TupleType const volatile>
	{
		typedef typename tuple_element<index, TupleType>::type const volatile type;
	};
} // namespace bcc

namespace bcc
{
	template<typename TupleType, typename T1>
	struct TupleCat;

#if BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename... Types, typename T1>
	struct TupleCat<tuple<Types...>, T1>
	{
		typedef tuple<Types..., T1> type;
	};

	template<typename... Types, typename T1>
	struct TupleCat<tuple<Types...> const, T1>
	{
		typedef tuple<Types..., T1> const type;
	};

	template<typename... Types, typename T1>
	struct TupleCat<tuple<Types...> volatile, T1>
	{
		typedef tuple<Types..., T1> volatile type;
	};

	template<typename... Types, typename T1>
	struct TupleCat<tuple<Types...> const volatile, T1>
	{
		typedef tuple<Types..., T1> const volatile type;
	};

#else // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename TupleType, typename T1>
	struct TupleCat
	{
		static size_t const size = tuple_size<TupleType>::value;
		static_assert(size < 30, "limit of tuple element count is 30.");

		template<size_t index, typename Tuple, typename Default = void>
		struct Helper
		{
			typedef typename Helper<index - 1, typename Tuple::Next, Default>::type type;
		};

		template<typename Tuple, typename Default>
		struct Helper<0, Tuple, Default>
		{
			typedef typename Tuple::Value type;
		};

		template<size_t index, typename Default>
		struct Helper<index, tuple<>, Default>
		{
			typedef Default type;
		};

		template<typename Default>
		struct Helper<0, tuple<>, Default>
		{
			typedef Default type;
		};

		typedef tuple<
			typename bcc::conditional<size == 0, T1, typename Helper<0, TupleType>::type>::type
			, typename bcc::conditional<size == 1, T1, typename Helper<1, TupleType>::type>::type
			, typename bcc::conditional<size == 2, T1, typename Helper<2, TupleType>::type>::type
			, typename bcc::conditional<size == 3, T1, typename Helper<3, TupleType>::type>::type
			, typename bcc::conditional<size == 4, T1, typename Helper<4, TupleType>::type>::type
			, typename bcc::conditional<size == 5, T1, typename Helper<5, TupleType>::type>::type
			, typename bcc::conditional<size == 6, T1, typename Helper<6, TupleType>::type>::type
			, typename bcc::conditional<size == 7, T1, typename Helper<7, TupleType>::type>::type
			, typename bcc::conditional<size == 8, T1, typename Helper<8, TupleType>::type>::type
			, typename bcc::conditional<size == 9, T1, typename Helper<9, TupleType>::type>::type
			, typename bcc::conditional<size == 10, T1, typename Helper<10, TupleType>::type>::type
			, typename bcc::conditional<size == 11, T1, typename Helper<11, TupleType>::type>::type
			, typename bcc::conditional<size == 12, T1, typename Helper<12, TupleType>::type>::type
			, typename bcc::conditional<size == 13, T1, typename Helper<13, TupleType>::type>::type
			, typename bcc::conditional<size == 14, T1, typename Helper<14, TupleType>::type>::type
			, typename bcc::conditional<size == 15, T1, typename Helper<15, TupleType>::type>::type
			, typename bcc::conditional<size == 16, T1, typename Helper<16, TupleType>::type>::type
			, typename bcc::conditional<size == 17, T1, typename Helper<17, TupleType>::type>::type
			, typename bcc::conditional<size == 18, T1, typename Helper<18, TupleType>::type>::type
			, typename bcc::conditional<size == 19, T1, typename Helper<19, TupleType>::type>::type
			, typename bcc::conditional<size == 20, T1, typename Helper<20, TupleType>::type>::type
			, typename bcc::conditional<size == 21, T1, typename Helper<21, TupleType>::type>::type
			, typename bcc::conditional<size == 22, T1, typename Helper<22, TupleType>::type>::type
			, typename bcc::conditional<size == 23, T1, typename Helper<23, TupleType>::type>::type
			, typename bcc::conditional<size == 24, T1, typename Helper<24, TupleType>::type>::type
			, typename bcc::conditional<size == 25, T1, typename Helper<25, TupleType>::type>::type
			, typename bcc::conditional<size == 26, T1, typename Helper<26, TupleType>::type>::type
			, typename bcc::conditional<size == 27, T1, typename Helper<27, TupleType>::type>::type
			, typename bcc::conditional<size == 28, T1, typename Helper<28, TupleType>::type>::type
			, typename bcc::conditional<size == 29, T1, typename Helper<29, TupleType>::type>::type
		> type;
	};

	template<typename TupleType, typename T1>
	struct TupleCat<TupleType const, T1>
	{
		typedef typename TupleCat<TupleType, T1>::type const type;
	};

	template<typename TupleType, typename T1>
	struct TupleCat<TupleType volatile, T1>
	{
		typedef typename TupleCat<TupleType, T1>::type volatile type;
	};

	template<typename TupleType, typename T1>
	struct TupleCat<TupleType const volatile, T1>
	{
		typedef typename TupleCat<TupleType, T1>::type const volatile type;
	};

#endif // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER

	template<typename TupleType, typename T, size_t n>
	struct tuple_cat_n
	{
		typedef typename tuple_cat_n<typename bcc::TupleCat<TupleType, T>::type, T, n - 1>::type type;
	};

	template<typename TupleType, typename T>
	struct tuple_cat_n<TupleType, T, 0>
	{
		typedef TupleType type;
	};

	template<typename TupleType, size_t position, typename T, size_t index = 0, size_t size = bcc::tuple_size<TupleType>::value, typename PreviousResult = bcc::tuple<> >
	struct tuple_replace
	{
		static_assert(position < size, "'position' must be less than 'size'.");
		typedef typename tuple_replace<TupleType, position, T, index + 1, size, typename bcc::TupleCat<PreviousResult, typename bcc::tuple_element<index, TupleType>::type>::type>::type type;
	};

	template<typename TupleType, size_t position, typename T, size_t size, typename PreviousResult>
	struct tuple_replace<TupleType, position, T, position, size, PreviousResult>
	{
		static_assert(position < size, "'position' must be less than 'size'.");
		typedef typename tuple_replace<TupleType, position, T, position + 1, size, typename bcc::TupleCat<PreviousResult, T>::type>::type type;
	};

	template<typename TupleType, size_t position, typename T, size_t size, typename PreviousResult>
	struct tuple_replace<TupleType, position, T, size, size, PreviousResult>
	{
		static_assert(position < size, "'position' must be less than 'size'.");
		typedef PreviousResult type;
	};	
} // namespace bcc

#endif // !def BICOMC_TUPLE_H__
