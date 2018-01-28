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

#ifndef BICOMC_DETAIL_COMPILER_CLANG_H__
#define BICOMC_DETAIL_COMPILER_CLANG_H__

#if !defined(__clang__)
#	error "compiler is not clang"
#endif // !def __clang__

#if !defined(BICOMC_EXPORT)
#	if defined(_MSC_VER) && defined(_WIN32)
#		define BICOMC_EXPORT __declspec(dllexport)
#	else
#		define BICOMC_EXPORT __attribute__((visibility("default")))
#	endif
#endif // !def BICOMC_EXPORT

#define BICOMC_CLANG_VERSION_OR_HIGHER(MAJOR, MINOR) \
	((__clang_major__ == MAJOR && __clang_minor__ >= MINOR) || (__clang_major__ > MAJOR))

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
#	define BICOMC_IS_CLANG_CXX_11_ENABLED (1)
#else
#	define BICOMC_IS_CLANG_CXX_11_ENABLED (0)
#endif

#if !defined(BICOMC_NO_NULLPTR)
#	if !__has_feature(cxx_nullptr)
#		define BICOMC_NO_NULLPTR
#	endif
#endif

#if !defined(BICOMC_NO_STATIC_ASSERT)
#	if !__has_feature(cxx_static_assert)
#		define BICOMC_NO_STATIC_ASSERT
#	endif
#endif

#if !defined(BICOMC_NO_THREAD_SAFE_STATIC_INIT)
#	if BICOMC_CLANG_VERSION_OR_HIGHER(2, 9)
#	elif defined(_MSC_VER) && _MSC_VER >= 1900
#	else
#		define BICOMC_NO_THREAD_SAFE_STATIC_INIT
#	endif
#endif

#if !defined(BICOMC_NO_CONSTEXPR)
#	if !__has_feature(cxx_constexpr)
#		define BICOMC_NO_CONSTEXPR
#	endif
#endif

#if !defined(BICOMC_NO_NOEXCEPT)
#	if !__has_feature(cxx_noexcept)
#		define BICOMC_NO_NOEXCEPT
#	endif
#endif

#if !defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE)
#	if !__has_feature(cxx_deleted_functions)
#		define BICOMC_NO_EXPLICIT_DEFAULT_DELETE
#	endif
#endif

#if !defined(BICOMC_NO_RVALUE_REFERENCE)
#	if !__has_feature(cxx_rvalue_references)
#		define BICOMC_NO_RVALUE_REFERENCE
#	endif
#endif

#if !defined(BICOMC_NO_VARIADIC_TEMPLATE)
#	if !__has_feature(cxx_variadic_templates)
#		define BICOMC_NO_VARIADIC_TEMPLATE
#	endif
#endif

#if !defined(BICOMC_NO_CHAR_16_32_TYPE)
#	if BICOMC_CLANG_VERSION_OR_HIGHER(2, 9) && BICOMC_IS_CLANG_CXX_11_ENABLED
#	elif defined(_MSC_VER) && _MSC_VER >= 1900
#	else
#		define BICOMC_NO_CHAR_16_32_TYPE
#	endif
#endif

#if !defined(BICOMC_NO_UNICODE_STRING_LITERAL)
#	if !__has_feature(cxx_unicode_literals)
#		define BICOMC_NO_UNICODE_STRING_LITERAL
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_CSTDINT)
#	if __has_include(<cstdint>) && (BICOMC_IS_CLANG_CXX_11_ENABLED || defined(_MSC_VER))
#	else
#		define BICOMC_NO_HEADER_CSTDINT
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_TYPE_TRAITS)
#	if __has_include(<type_traits>) && (BICOMC_IS_CLANG_CXX_11_ENABLED || defined(_MSC_VER))
#	else
#		define BICOMC_NO_HEADER_TYPE_TRAITS
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_CODECVT)
#	if __has_include(<codecvt>) && (BICOMC_IS_CLANG_CXX_11_ENABLED || defined(_MSC_VER))
#	else
#		define BICOMC_NO_HEADER_CODECVT
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_ARRAY)
#	if __has_include(<array>) && (BICOMC_IS_CLANG_CXX_11_ENABLED || defined(_MSC_VER))
#	else
#		define BICOMC_NO_HEADER_ARRAY
#	endif
#endif

#undef BICOMC_IS_CLANG_CXX_11_ENABLED
#undef BICOMC_CLANG_VERSION_OR_HIGHER

#endif // !def BICOMC_DETAIL_COMPILER_CLANG_H__
