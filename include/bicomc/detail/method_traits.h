#ifndef BICOMC_DETAIL_METHOD_TRAITS_H__
#define BICOMC_DETAIL_METHOD_TRAITS_H__

#include "config.h"

#include "../tuple.h"
#include "../type_traits.h"

#include "method_return.h"

namespace bcc
{
namespace detail
{
	class ErrorDetail;

	template<typename Function, typename Owner = void>
	struct MethodTypeTrait;

	template<typename Function, typename Owner, bool isConst = bcc::is_const<Owner>::value, bool isVolatile = bcc::is_volatile<Owner>::value>
	struct MethodTypeDeducer
	{
		template<typename NewOwner>
		struct change_owner
		{
			typedef MethodTypeDeducer<Function, NewOwner, isConst, isVolatile> type;
			typedef typename type::raw_owner raw_onwer;
			typedef typename type::owner onwer;
			typedef typename type::trait trait;
			typedef typename type::member member;
			typedef typename type::helper helper;
		};

		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			isConst && isVolatile, raw_owner const volatile
			, typename bcc::conditional<isConst, raw_owner const
				, typename bcc::conditional<isVolatile, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef MethodTypeTrait<Function, owner> trait;

		typedef typename bcc::conditional<
			isConst && isVolatile, typename trait::member_cv
			, typename bcc::conditional<isConst, typename trait::member_const
				, typename bcc::conditional<isVolatile, typename trait::member_volatile
			, typename trait::member>::type>::type>::type member;

		typedef typename bcc::conditional<
			isConst && isVolatile, typename trait::helper_cv
			, typename bcc::conditional<isConst, typename trait::helper_const
				, typename bcc::conditional<isVolatile, typename trait::helper_volatile
			, typename trait::helper>::type>::type>::type helper;
	};

#if BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename Owner, typename Ret, typename... Params>
	struct MethodTypeTrait<Ret(Params...), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<Params...> params;
		typedef ret(type)(Params...);


		typedef ret(raw_owner::*member)(Params...);
		typedef ret(raw_owner::*member_const)(Params...) const;
		typedef ret(raw_owner::*member_volatile)(Params...) volatile;
		typedef ret(raw_owner::*member_cv)(Params...) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, Params...);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, Params...);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, Params...);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, Params...);
	};

	template<typename Ret, typename... Params>
	struct MethodTypeTrait<Ret(Params...), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<Params...> params;
		typedef ret(type)(Params...);
	};

#else // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename Owner, typename Ret>
	struct MethodTypeTrait<Ret(), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<> params;
		typedef ret(type)();
		
		typedef ret(raw_owner::*member)();
		typedef ret(raw_owner::*member_const)() const;
		typedef ret(raw_owner::*member_volatile)() volatile;
		typedef ret(raw_owner::*member_cv)() const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*);
	};

	template<typename Ret>
	struct MethodTypeTrait<Ret(), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<> params;
		typedef ret(type)();
	};

	template<typename Owner, typename Ret, typename P1>
	struct MethodTypeTrait<Ret(P1), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1> params;
		typedef ret(type)(P1);

		typedef ret(raw_owner::*member)(P1);
		typedef ret(raw_owner::*member_const)(P1) const;
		typedef ret(raw_owner::*member_volatile)(P1) volatile;
		typedef ret(raw_owner::*member_cv)(P1) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1);
	};

	template<typename Ret, typename P1>
	struct MethodTypeTrait<Ret(P1), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1> params;
		typedef ret(type)(P1);
	};

	template<typename Owner, typename Ret, typename P1, typename P2>
	struct MethodTypeTrait<Ret(P1, P2), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2> params;
		typedef ret(type)(P1, P2);

		typedef ret(raw_owner::*member)(P1, P2);
		typedef ret(raw_owner::*member_const)(P1, P2) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2);
	};

	template<typename Ret, typename P1, typename P2>
	struct MethodTypeTrait<Ret(P1, P2), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2> params;
		typedef ret(type)(P1, P2);
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3>
	struct MethodTypeTrait<Ret(P1, P2, P3), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3> params;
		typedef ret(type)(P1, P2, P3);

		typedef ret(raw_owner::*member)(P1, P2, P3);
		typedef ret(raw_owner::*member_const)(P1, P2, P3) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3);
	};

	template<typename Ret, typename P1, typename P2, typename P3>
	struct MethodTypeTrait<Ret(P1, P2, P3), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3> params;
		typedef ret(type)(P1, P2, P3);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4> params;
		typedef ret(type)(P1, P2, P3, P4);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4> params;
		typedef ret(type)(P1, P2, P3, P4);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5> params;
		typedef ret(type)(P1, P2, P3, P4, P5);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5> params;
		typedef ret(type)(P1, P2, P3, P4, P5);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7, P8), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7, P8), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9>
	struct MethodTypeTrait<Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28, typename P29>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28
			, P29), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28, P29> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28, P29);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28, typename P29>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28
			, P29), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28, P29> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28, P29);
	};

	template<typename Owner, typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28, typename P29, typename P30>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28
			, P29, P30), Owner>
	{
		typedef typename bcc::remove_cv<typename bcc::remove_reference<Owner>::type>::type raw_owner;
		typedef typename bcc::conditional<
			bcc::is_const<Owner>::value && bcc::is_volatile<Owner>::value, raw_owner const volatile
			, typename bcc::conditional<bcc::is_const<Owner>::value, raw_owner const
				, typename bcc::conditional<bcc::is_volatile<Owner>::value, raw_owner volatile
			, raw_owner>::type>::type>::type owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28, P29, P30> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28, P29, P30);

		typedef ret(raw_owner::*member)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30);
		typedef ret(raw_owner::*member_const)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30) const;
		typedef ret(raw_owner::*member_volatile)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30) volatile;
		typedef ret(raw_owner::*member_cv)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30) const volatile;

		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper)(raw_owner*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_const)(raw_owner const*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_volatile)(raw_owner volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30);
		typedef bcc::detail::ErrorDetail*(BICOMC_CALL helper_cv)(raw_owner const volatile*, typename ReturnHelper<ret>::mediator*, P1, P2, P3, P4, P5, P6, P7, P8, P9, P10
			, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23
			, P24, P25, P26, P27, P28, P29, P30);
	};

	template<typename Ret
		, typename P1, typename P2, typename P3, typename P4, typename P5
		, typename P6, typename P7, typename P8, typename P9, typename P10
		, typename P11, typename P12, typename P13, typename P14, typename P15
		, typename P16, typename P17, typename P18, typename P19, typename P20
		, typename P21, typename P22, typename P23, typename P24, typename P25
		, typename P26, typename P27, typename P28, typename P29, typename P30>
	struct MethodTypeTrait<
		Ret(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15
			, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28
			, P29, P30), void>
	{
		typedef void raw_owner;
		typedef void owner;

		typedef Ret ret;
		typedef bcc::tuple<P1, P2, P3, P4, P5, P6, P7, P8, P9
			, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22
			, P23, P24, P25, P26, P27, P28, P29, P30> params;
		typedef ret(type)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12
			, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24
			, P25, P26, P27, P28, P29, P30);
	};

#endif // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_METHOD_TRAITS_H__
