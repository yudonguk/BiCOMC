/*
 * Copyright 2016 Kangwon National University
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

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "msvc.h"

#elif defined(__GNUC__) && !defined(__INTEL_COMPILER) && !defined(__clang__)
#	include "gcc.h"

#elif defined(__INTEL_COMPILER)
#	include "intel.h"

#elif defined(__clang__)
#	include "clang.h"

#endif

//////////////////////////////////////////////////////////////////////////

#if defined(nullptr)
#	undef BICOMC_IS_NULLPTR_SUPPORT_COMPILER
#	define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
		(1)
#else
#	if !defined(BICOMC_IS_NULLPTR_SUPPORT_COMPILER)
#		define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
			(__cplusplus >= 201103L)
#	endif // !def BICOMC_IS_NULLPTR_SUPPORT_COMPILER
#endif // def nullptr

#if defined(nullptr)
#	undef BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
#	define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
		(1)
#else
#	if !defined(BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER)
#		define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
			(__cplusplus >= 201103L)
#	endif // !def BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
#endif // def static_assert

#if !defined(BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER)
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER

#if !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER

#if !defined(BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER)
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER

#if !defined(BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER)
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER

#if !defined(BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER)
#	define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

#if !defined(BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER)
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER

#if !defined(BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER)
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER

#if !defined(BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER)
#	define BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

#if !defined(BICOMC_IS_STD_INT_SUPPORT_COMPILER)
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_STD_INT_SUPPORT_COMPILER

#if !defined(BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER)
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER

#if !defined(BICOMC_IS_MUTEX_SUPPORT_COMPILER)
#	define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_MUTEX_SUPPORT_COMPILER

#if !defined(BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER)
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER

#if !defined(BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER)
#	define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER

#if !defined(BICOMC_IS_ARRAY_SUPPORT_COMPILER)
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_ARRAY_SUPPORT_COMPILER

#endif // !def BICOMC_DETAIL_COMPILER_COMPILER_H__
