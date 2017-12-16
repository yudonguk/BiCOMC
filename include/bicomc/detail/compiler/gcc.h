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

#ifndef BICOMC_DETAIL_COMPILER_GCC_H__
#define BICOMC_DETAIL_COMPILER_GCC_H__

#if !defined(__GNUC__)
#	error "compiler is not gcc"
#endif // !def __GNUC__

#if !defined(BICOMC_EXPORT)
#	if defined(_WIN32)
#		define BICOMC_EXPORT __attribute__((dllexport))
#	else
#		define BICOMC_EXPORT __attribute__((visibility("default")))
#	endif
#endif // !def BICOMC_EXPORT

#if !defined(BICOMC_IS_NULLPTR_SUPPORT_COMPILER)
#	define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_NULLPTR_SUPPORT_COMPILER
#if !defined(BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER)
#	define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER)
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__) || __GNUC__ >= 4)
#endif // !def BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER)
#	define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER
#if !defined(BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER)
#	define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER)
#	define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER
#if !defined(BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER)
#	define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
#if !defined(BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER)
#	define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER)
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER
#if !defined(BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER)
#	define BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#if !defined(BICOMC_IS_STD_INT_SUPPORT_COMPILER)
#	define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_STD_INT_SUPPORT_COMPILER
#if !defined(BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER)
#	define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER	\
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER
#if !defined(BICOMC_IS_MUTEX_SUPPORT_COMPILER)
#	define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_MUTEX_SUPPORT_COMPILER
#if !defined(BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER)
#	define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
		((__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)) && __GNUC__ > 4)
#endif // !def BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
#if !defined(BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER)
#	define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER
#if !defined(BICOMC_IS_ARRAY_SUPPORT_COMPILER)
#	define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif // !def BICOMC_IS_ARRAY_SUPPORT_COMPILER

#endif // !def BICOMC_DETAIL_COMPILER_GCC_H__
