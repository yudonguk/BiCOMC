#ifndef BICOMC_DETAIL_CONFIG_H__
#define BICOMC_DETAIL_CONFIG_H__

#include <cstddef>

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#	include "compiler/msvc.h"

#elif defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__) 
#	include "compiler/gcc.h"

#elif defined(__INTEL_COMPILER)
#	include "compiler/intel.h"

#else
#	error "compiler is not supported"

#endif // defined(_MSC_VER) && !defined(__INTEL_COMPILER)

//////////////////////////////////////////////////////////////////////////

#if !defined(BICOMC_CALL) || !defined(BICOMC_EXPORT)
#	error "compiler is not supported"
#endif // !defined(BICOMC_CALL) && !defined(BICOMC_EXPORT)

#if !defined(BICOMC_IS_NULLPTR_SUPPORT_COMPILER)
#	if defined(nullptr)
#		define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
			(1)
#	else
#		define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
			(__cplusplus >= 201103L)
#	endif // defined(nullptr)
#endif // !defined(BICOMC_IS_NULLPTR_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER)
#	if defined(static_assert)
#		define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
			(1)
#	else
#		define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
			(__cplusplus >= 201103L)
#	endif // defined(static_assert)
#endif // !defined(BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER)
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER)
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER)
#	define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER)
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER)
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_STD_INT_SUPPORT_COMPILER)
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_STD_INT_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER)
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER	\
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_MUTEX_SUPPORT_COMPILER)
#	define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_MUTEX_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER)
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER)
#	define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER)

#if !defined(BICOMC_IS_ARRAY_SUPPORT_COMPILER)
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !defined(BICOMC_IS_ARRAY_SUPPORT_COMPILER)

//////////////////////////////////////////////////////////////////////////

#define BICOMC_WSTRINGIZER2(str) L ## str
#define BICOMC_WSTRINGIZER(str) BICOMC_WSTRINGIZER2(#str)

#define BICOMC_STRINGIZER2(str) str
#define BICOMC_STRINGIZER(str) BICOMC_STRINGIZER2(#str)

#if BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER
#	define BICOMC_CONSTEXPR constexpr
#else
#	define BICOMC_CONSTEXPR 
#endif // BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER

#if BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER
#	define BICOMC_NOEXCEPT(...) noexcept(__VA_ARGS__)
#else
#	define BICOMC_NOEXCEPT(...)
#endif // BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER

#if BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER
#	define BICOMC_DELETE = delete
#else
#	define BICOMC_DELETE
#endif // BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER

#if !BICOMC_IS_NULLPTR_SUPPORT_COMPILER
namespace bcc
{
namespace detail
{
	class nullptr_t
	{
	public:
		template<typename T>
		inline operator T*() const
		{
			return 0;
		}

		template<typename U, typename T>
		inline operator T U::*() const
		{
			return 0;
		}

	private:
		void operator&() const;
	};
} // namespace detail
} // namespace bcc

namespace std
{
	using bcc::detail::nullptr_t;
} // namespace std

#define nullptr std::nullptr_t()

#endif // !BICOMC_IS_NULLPTR_SUPPORT_COMPILER

#if !BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
namespace bcc
{
namespace detail
{
	template<bool condition>
	struct StaticAssertor;

	template<>
	struct StaticAssertor<true>
	{};

	template<size_t i>
	struct StaticAssertTester
	{};
} // namespace detail
} // namespace bcc

#define BICOMC_STATIC_ASSERT_TYPE_NAME2(name, number) name ## number
#define BICOMC_STATIC_ASSERT_TYPE_NAME(name, number) BICOMC_STATIC_ASSERT_TYPE_NAME2(name, number)

#define static_assert(condition, message) \
	typedef bcc::detail::StaticAssertTester<sizeof(bcc::detail::StaticAssertor<!!(condition)>)> BICOMC_STATIC_ASSERT_TYPE_NAME(bicomc_static_assert_, __COUNTER__)

#endif // !BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER

#endif // !def BICOMC_DETAIL_CONFIG_H__
