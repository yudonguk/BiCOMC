/*
 * Copyright 2016 Kangwon National University and Contributors
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

#ifndef BICOMC_DETAIL_COMPILER_COMPILER_H__
#define BICOMC_DETAIL_COMPILER_COMPILER_H__

#include <cstddef>

#define BICOMC_MACRO_OR_HIGHER(NAME, VALUE) (defined(NAME) && (NAME >= VALUE))

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "msvc.h"

#elif defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "gcc.h"

#elif defined(__INTEL_COMPILER)
#	include "intel.h"

#elif defined(__clang__)
#	include "clang.h"

#else
#	if !defined(BICOMC_NO_NULLPTR) && (__cplusplus < 201103L)
#		define BICOMC_NO_NULLPTR
#	endif
#	if !defined(BICOMC_NO_STATIC_ASSERT) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_static_assert, 200410)
#		define BICOMC_NO_STATIC_ASSERT
#	endif
#	if !defined(BICOMC_NO_THREAD_SAFE_STATIC_INIT) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_threadsafe_static_init, 200806)
#		define BICOMC_NO_THREAD_SAFE_STATIC_INIT
#	endif
#	if !defined(BICOMC_NO_CONSTEXPR) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_constexpr, 200704)
#		define BICOMC_NO_CONSTEXPR
#	endif
#	if !defined(BICOMC_NO_NOEXCEPT) && (__cplusplus < 201103L)
#		define BICOMC_NO_NOEXCEPT
#	endif
#	if !defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE) && (__cplusplus < 201103L)
#		define BICOMC_NO_EXPLICIT_DEFAULT_DELETE
#	endif
#	if !defined(BICOMC_NO_RVALUE_REFERENCE) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(_cpp_rvalue_references, 200610)
#		define BICOMC_NO_RVALUE_REFERENCE
#	endif
#	if !defined(BICOMC_NO_VARIADIC_TEMPLATE) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_variadic_templates, 200704)
#		define BICOMC_NO_VARIADIC_TEMPLATE
#	endif
#	if !defined(BICOMC_NO_CHAR_16_32_TYPE) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_unicode_characters, 200704)
#		define BICOMC_NO_CHAR_16_32_TYPE
#	endif
#	if !defined(BICOMC_NO_UNICODE_STRING_LITERAL) && (__cplusplus < 201103L) \
		&& !BICOMC_MACRO_OR_HIGHER(__cpp_unicode_literals, 200710)
#		define BICOMC_NO_UNICODE_STRING_LITERAL
#	endif
#	if !defined(BICOMC_NO_HEADER_CSTDINT) && (__cplusplus < 201103L)
#		define BICOMC_NO_HEADER_CSTDINT
#	endif
#	if !defined(BICOMC_NO_HEADER_TYPE_TRAITS) && (__cplusplus < 201103L)
#		define BICOMC_NO_HEADER_TYPE_TRAITS
#	endif
#	if !defined(BICOMC_NO_HEADER_CODECVT) && (__cplusplus < 201103L)
#		define BICOMC_NO_HEADER_CODECVT
#	endif
#	if !defined(BICOMC_NO_HEADER_ARRAY) && (__cplusplus < 201103L)
#		define BICOMC_NO_HEADER_ARRAY
#	endif

#endif

#if defined(nullptr)
#	undef BICOMC_NO_NULLPTR
#endif // def nullptr

#if defined(static_assert)
#	undef BICOMC_NO_STATIC_ASSERT
#endif // def static_assert

//////////////////////////////////////////////////////////////////////////

#if defined(BICOMC_NO_NULLPTR)
#	define BICOMC_IS_NULLPTR_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_NULLPTR_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_STATIC_ASSERT)
#	define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_THREAD_SAFE_STATIC_INIT)
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_CONSTEXPR)
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_NOEXCEPT)
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE)
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_RVALUE_REFERENCE)
#	define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_VARIADIC_TEMPLATE)
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_CHAR_16_32_TYPE)
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_UNICODE_STRING_LITERAL)
#	define BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_HEADER_CSTDINT)
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_HEADER_TYPE_TRAITS)
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_HEADER_CODECVT)
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER (1)
#endif

#if defined(BICOMC_NO_HEADER_ARRAY)
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER (0)
#else
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER (1)
#endif

#undef BICOMC_MACRO_OR_HIGHER

#endif // !def BICOMC_DETAIL_COMPILER_COMPILER_H__
