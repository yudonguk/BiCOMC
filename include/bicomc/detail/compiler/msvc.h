﻿/*
 * Copyright 2015 Kangwon National University
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

#ifndef BICOMC_DETAIL_COMPILER_MSVC_H__
#define BICOMC_DETAIL_COMPILER_MSVC_H__

#if !defined(_MSC_VER)
#	error "compiler is not msvc"
#endif // !def _MSC_VER

#pragma warning(disable:4503)

#if !defined(BICOMC_EXPORT)
#	define BICOMC_EXPORT __declspec(dllexport)
#endif // !def BICOMC_EXPORT

#if !defined(BICOMC_IS_NULLPTR_SUPPORT_COMPILER)
#	define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_NULLPTR_SUPPORT_COMPILER
#if !defined(BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER)
#	define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER)
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(_MSC_VER >= 1900)
#endif // !def BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
		(_MSC_VER >= 1900)
#endif // !def BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER
#if !defined(BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER)
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
		(_MSC_VER >= 1900)
#endif // !def BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER)
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
		(_MSC_VER >= 1800)
#endif // !def BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER
#if !defined(BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER)
	#define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#if !defined(BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER)
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
		(_MSC_VER > 1800)
#endif // !def BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER)
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(_MSC_VER >= 1900)
#endif // !def BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER
#if !defined(BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER)
#	define BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER \
		(_MSC_VER >= 1900)
#endif // !def BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#if !defined(BICOMC_IS_STD_INT_SUPPORT_COMPILER)
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_STD_INT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER)
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER
#if !defined(BICOMC_IS_MUTEX_SUPPORT_COMPILER)
#	define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
		(_MSC_VER >= 1700)
#endif // !def BICOMC_IS_MUTEX_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER)
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
		(_MSC_VER >= 1700)
#endif // !def BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
#if !defined(BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER)
#	define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L)
#endif // !def BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER
#if !defined(BICOMC_IS_ARRAY_SUPPORT_COMPILER)
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
		(_MSC_VER >= 1600)
#endif // !def BICOMC_IS_ARRAY_SUPPORT_COMPILER

#endif // !def BICOMC_DETAIL_COMPILER_MSVC_H__
