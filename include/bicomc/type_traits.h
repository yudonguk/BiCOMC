#ifndef BICOMC_TYPE_TRAITS_H__
#define BICOMC_TYPE_TRAITS_H__

#include "detail/config.h"

#include "stdint.h"

#if BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER

#include <type_traits>

namespace bcc
{
	using std::integral_constant;
	using std::true_type;
	using std::false_type;

	using std::remove_cv;
	using std::remove_const;
	using std::remove_volatile;
	using std::remove_reference;

	using std::conditional;
	using std::enable_if;

	using std::is_same;
	using std::is_volatile;
	using std::is_integral;
	using std::is_floating_point;
	using std::is_arithmetic;
	using std::is_pointer;
	using std::is_const;
	using std::is_function;
	using std::is_reference;
	using std::is_void;
	using std::is_fundamental;
	using std::is_signed;
	using std::is_unsigned;

	using std::add_const;
	using std::add_volatile;
	using std::add_cv;
}

#else

#include <limits>

namespace bcc
{
	template<typename T, T value_>
	struct integral_constant
	{
		typedef	T value_type;
		typedef integral_constant<T, value_> type;

		static const value_type value = value_;

		operator value_type() const { return value; }
		value_type operator()() const { return value; }
	};

	typedef integral_constant<bool, true> true_type;
	typedef integral_constant<bool, false> false_type;
	
	template<typename T>
	struct remove_const
	{
		typedef T type;
	};

	template<typename T>
	struct remove_const<T const>
	{
		typedef T type;
	};

	template<typename T>
	struct remove_volatile
	{
		typedef T type;
	};

	template<typename T>
	struct remove_volatile<T volatile>
	{
		typedef T type;
	};

	template<typename T>
	struct remove_cv
	{
		typedef typename remove_volatile<typename remove_const<T>::type>::type type;
	};
		
	template<typename T>
	struct remove_reference
	{
		typedef T type;
	};

	template<typename T>
	struct remove_reference<T&>
	{
		typedef T type;
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct remove_reference<T&&>
	{
		typedef T type;
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	
	template<bool condition, typename T, typename U>
	struct conditional
	{
		typedef T type;
	};

	template<typename T, typename U>
	struct conditional<false, T, U>
	{
		typedef U type;
	};

	template<bool codition, typename T = void>
	struct enable_if
	{};

	template<typename T>
	struct enable_if<true, T>
	{
		typedef T type;
	};

	template<typename T, typename U>
	struct is_same : false_type
	{};

	template<typename T>
	struct is_same<T, T> : true_type
	{};

	template<typename T>
	struct is_volatile : public false_type
	{};

	template<typename T>
	struct is_volatile<T volatile> : public true_type
	{};

namespace detail
{
	template<typename T>
	struct is_integral_impl : public false_type
	{};

	template<>
	struct is_integral_impl<bool> : public true_type
	{};

	template<>
	struct is_integral_impl<char> : public true_type
	{};

	template<>
	struct is_integral_impl<signed char> : public true_type
	{};

	template<>
	struct is_integral_impl<unsigned char> : public true_type
	{};

	template<>
	struct is_integral_impl<wchar_t> : public true_type
	{};

#if BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER
	template<>
	struct is_integral_impl<char16_t> : public true_type
	{};

	template<>
	struct is_integral_impl<char32_t> : public true_type
	{};
#endif

	template<>
	struct is_integral_impl<signed short int> : public true_type
	{};

	template<>
	struct is_integral_impl<unsigned short int> : public true_type
	{};

	template<>
	struct is_integral_impl<signed int> : public true_type
	{};

	template<>
	struct is_integral_impl<unsigned int> : public true_type
	{};

	template<>
	struct is_integral_impl<signed long int> : public true_type
	{};

	template<>
	struct is_integral_impl<unsigned long int> : public true_type
	{};

#if defined(LLONG_MAX)
	template<>
	struct is_integral_impl<signed long long int> : public true_type
	{};
#endif // defined(LLONG_MAX)

#if defined(ULLONG_MAX)
	template<>
	struct is_integral_impl<unsigned long long int> : public true_type
	{};
#endif // defined(ULLONG_MAX)
} // namespace detail

	template<typename T>
	struct is_integral : public detail::is_integral_impl<typename remove_cv<T>::type>
	{};
	
namespace detail
{
	template<typename T>
	struct is_floating_point_impl : public false_type
	{};

	template<>
	struct is_floating_point_impl<float> : public true_type
	{};

	template<>
	struct is_floating_point_impl<double> : public true_type
	{};

#if defined(LDBL_MAX)
	template<>
	struct is_floating_point_impl<long double> : public true_type
	{};
#endif // #if defined(LDBL_MAX)
} // namesapce detail

	template<typename T>
	struct is_floating_point : public detail::is_floating_point_impl<typename remove_cv<T>::type>
	{};

	template<typename T>
	struct is_arithmetic : public integral_constant<bool, is_integral<T>::value || is_floating_point<T>::value>
	{};
		
namespace detail
{
	template<typename T>
	struct is_pointer_impl : public false_type
	{};

	template<typename T>
	struct is_pointer_impl<T*> : public true_type
	{};
} // namespace detail

	template<typename T>
	struct is_pointer : public detail::is_pointer_impl<typename remove_cv<T>::type>
	{};

	template<typename T>
	struct is_const : public false_type
	{};

	template<typename T>
	struct is_const<T const> : public true_type
	{};

namespace detail
{
	template<typename T>
	struct is_function_impl
	{
		typedef bcc::int8_t TrueType;
		typedef bcc::int16_t FalseType;
		
		template<typename U> static FalseType test(int U::*);
		template<typename U> static TrueType test(...);
		
		template<typename U> static FalseType test2(U*, typename bcc::enable_if<sizeof(U())>::type* = 0);
		template<typename U> static TrueType test2(...);
				
		static bool const value = !bcc::is_integral<T>::value
			&& sizeof(test<T>(0)) == sizeof(TrueType) 
			&& sizeof(test2<T>(0)) == sizeof(TrueType);
	};

	template<>
	struct is_function_impl<void>
	{
		static bool const value = false;
	};

	template<typename T>
	struct is_function_impl<T*>
	{
		static bool const value = false;
	};

	template<typename T>
	struct is_function_impl<T&> : public is_function_impl<T>
	{};
} // namespace detail

	template<typename T>
	struct is_function : public integral_constant<bool, detail::is_function_impl<T>::value>
	{};

	template<typename T>
	struct is_reference : public false_type
	{};

	template<typename T>
	struct is_reference<T&> : public true_type
	{};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct is_reference<T&&> : public true_type
	{};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	template<typename T>
	struct is_void : public is_same<void, typename remove_cv<T>::type>
	{};

	template<typename T>
	struct is_fundamental 
		: public integral_constant<bool, is_arithmetic<T>::value 
			|| is_void<T>::value 
			|| is_same<std::nullptr_t, typename remove_cv<T>::type>::value>
	{};

namespace detail
{
	template<typename T, bool isArithmetic = bcc::is_arithmetic<T>::value>
	struct is_signed_impl
		: public bcc::integral_constant<bool, T(-1) < T(0)>
	{};

	template<typename T>
	struct is_signed_impl<T, false> : public bcc::false_type
	{};
} // namespace detail

	template<typename T>
	struct is_signed : public integral_constant<bool, detail::is_signed_impl<T>::value>
	{};

	template<typename T>
	struct is_unsigned : public integral_constant<bool, !is_signed<T>::value>
	{};

	template<typename T>
	struct add_const
	{
		typedef T const type;
	};

	template<typename T>
	struct add_volatile
	{
		typedef T volatile type;
	};

	template<typename T>
	struct add_cv
	{
		typedef T const volatile type;
	};
}

#endif // BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER

namespace bcc
{
	template<typename T>
	struct is_character
		: public integral_constant<bool
			, is_same<T, char>::value
			|| is_same<T, signed char>::value
			|| is_same<T, wchar_t>::value
#if BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER
			|| is_same<T, char16_t>::value
			|| is_same<T, char32_t>::value
#endif // BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER
			>
	{};
} // namespace bcc

namespace bcc
{
	template<typename T>
	struct remove_cv_all
	{
		typedef typename remove_cv<T>::type type;
	};

	template<typename T>
	struct remove_cv_all<T*>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T volatile*>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T* volatile>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T* const>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T volatile* const>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T* volatile const>
	{
		typedef typename remove_cv_all<T>::type* type;
	};

	template<typename T>
	struct remove_cv_all<T&>
	{
		typedef typename remove_cv_all<T>::type type;
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct remove_cv_all<T&&>
	{
		typedef typename remove_cv_all<T>::type type;
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
} // namespace bcc

#endif // !def BICOMC_TYPE_TRAITS_H__
