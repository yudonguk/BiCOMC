/*
 * Copyright 2015 Kangwon National University and Contributors
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef BICOMC_DETAIL_CONFIG_H__
#define BICOMC_DETAIL_CONFIG_H__

#include <cstddef>

#if !defined(BICOMC_CALL)
#	include "compiler/bicomc_call.h"
#endif // !def BICOMC_CALL
#include "compiler/compiler.h"

#define BICOMC_VERSION 1

#if !defined(BICOMC_CALL)
#	define BICOMC_CALL
#endif // !def BICOMC_CALL

#if !defined(BICOMC_EXPORT)
#	define BICOMC_EXPORT
#endif // !def BICOMC_EXPORT

//////////////////////////////////////////////////////////////////////////

#define BICOMC_MACRO_PASS2(...) __VA_ARGS__
#define BICOMC_MACRO_PASS(PARAMETER) BICOMC_MACRO_PASS2 PARAMETER

#define BICOMC_WSTRINGIZER2(str) L ## str
#define BICOMC_WSTRINGIZER(...) BICOMC_WSTRINGIZER2(#__VA_ARGS__)

#if !defined(BICOMC_NO_UNICODE_STRING_LITERAL)
#	define BICOMC_STRINGIZER2(str) u8 ## str
#else
#	define BICOMC_STRINGIZER2(str) str
#endif // !def BICOMC_NO_UNICODE_STRING_LITERAL
#define BICOMC_STRINGIZER(...) BICOMC_STRINGIZER2(#__VA_ARGS__)

#if defined(__COUNTER__)
#	define BICOMC_LINE_COUNTER __COUNTER__
#else
#	define BICOMC_LINE_COUNTER __LINE__
#endif // !def __COUNTER__

#if !defined(BICOMC_NO_CONSTEXPR)
#	define BICOMC_CONSTEXPR constexpr
#else
#	define BICOMC_CONSTEXPR
#endif // !def BICOMC_NO_CONSTEXPR

#if __cplusplus >= 201402L || (defined(__cpp_constexpr) && __cpp_constexpr >= 201304)
#	define BICOMC_CONSTEXPR_14 BICOMC_CONSTEXPR
#else
#	define BICOMC_CONSTEXPR_14
#endif // __cplusplus >= 201402L

#if __cplusplus >= 201703L || (defined(__cpp_constexpr) && __cpp_constexpr >= 201603)
#	define BICOMC_CONSTEXPR_17 BICOMC_CONSTEXPR
#else
#	define BICOMC_CONSTEXPR_17
#endif // __cplusplus >= 201703L

#if !defined(BICOMC_NO_NOEXCEPT)
#	define BICOMC_NOEXCEPT noexcept
#	define BICOMC_NOEXCEPT_EX(...) noexcept(noexcept(__VA_ARGS__))
#else
#	define BICOMC_NOEXCEPT throw()
#	define BICOMC_NOEXCEPT_EX(...)
#endif // !def BICOMC_NO_NOEXCEPT

#if !defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE)
#	define BICOMC_DELETE = delete
#else
#	define BICOMC_DELETE
#endif // !def BICOMC_NO_EXPLICIT_DEFAULT_DELETE

#if defined(BICOMC_NO_NULLPTR)
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

#endif // !def BICOMC_NO_NULLPTR

#if !defined(BICOMC_NO_STATIC_ASSERT)
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

	template<std::size_t i>
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

#endif // !def BICOMC_NO_STATIC_ASSERT

#endif // !def BICOMC_DETAIL_CONFIG_H__
