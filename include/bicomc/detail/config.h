#ifndef BICOMC_DETAIL_CONFIG_H__
#define BICOMC_DETAIL_CONFIG_H__

#include <cstddef>

#if !defined(BICOMC_CALL)
#	include "compiler/bicomc_call.h"
#endif // !def BICOMC_CALL
#include "compiler/compiler.h"

#define BICOMC_VERSION 1

#if !defined(BICOMC_CALL)
#	error "compiler is not supported"
#endif // !def BICOMC_CALL

#if !defined(BICOMC_EXPORT)
#	define BICOMC_EXPORT
#endif // !def BICOMC_EXPORT

//////////////////////////////////////////////////////////////////////////

#define BICOMC_MACRO_PASS2(...) __VA_ARGS__
#define BICOMC_MACRO_PASS(PARAMETER) BICOMC_MACRO_PASS2 PARAMETER

#define BICOMC_WSTRINGIZER2(str) L ## str
#define BICOMC_WSTRINGIZER(...) BICOMC_WSTRINGIZER2(#__VA_ARGS__)

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#	define BICOMC_STRINGIZER2(str) u8 ## str
#else
#	define BICOMC_STRINGIZER2(str) str
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#define BICOMC_STRINGIZER(...) BICOMC_STRINGIZER2(#__VA_ARGS__)

#if defined(__COUNTER__)
#	define BICOMC_LINE_COUNTER __COUNTER__
#else
#	define BICOMC_LINE_COUNTER __LINE__
#endif // !def __COUNTER__

#if BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER
#	define BICOMC_CONSTEXPR constexpr
#else
#	define BICOMC_CONSTEXPR 
#endif // BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER

#if BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER
#	define BICOMC_NOEXCEPT noexcept
#	define BICOMC_NOEXCEPT_EX(...) noexcept(noexcept(__VA_ARGS__))
#else
#	define BICOMC_NOEXCEPT throw()
#	define BICOMC_NOEXCEPT_EX(...)
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
		inline BICOMC_CONSTEXPR operator T*() const BICOMC_NOEXCEPT
		{
			return 0;
		}

		template<typename U, typename T>
		inline BICOMC_CONSTEXPR operator T U::*() const BICOMC_NOEXCEPT
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

#	define nullptr std::nullptr_t()

#endif // !BICOMC_IS_NULLPTR_SUPPORT_COMPILER

#if BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
#	define BICOMC_STATIC_ASSERT(CONDITION, MESSAGE, HINT_NAME) \
		static_assert(CONDITION, MESSAGE)

#else
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

#	define BICOMC_STATIC_ASSERT_TYPE_NAME2(NAME, NUMBER) NAME ## NUMBER ## __
#	define BICOMC_STATIC_ASSERT_TYPE_NAME(NAME, NUMBER) BICOMC_STATIC_ASSERT_TYPE_NAME2(NAME, NUMBER)

#	define BICOMC_STATIC_ASSERT(CONDITION, MESSAGE, HINT_NAME) \
		typedef bcc::detail::StaticAssertTester<sizeof(bcc::detail::StaticAssertor<!!(CONDITION)>)> \
			BICOMC_STATIC_ASSERT_TYPE_NAME(BiCOMC_static_assert_ ## HINT_NAME ## _, BICOMC_LINE_COUNTER)

#	define static_assert(CONDITION, MESSAGE) \
		BICOMC_STATIC_ASSERT(CONDITION, MESSAGE, )

#endif // BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER

#endif // !def BICOMC_DETAIL_CONFIG_H__
