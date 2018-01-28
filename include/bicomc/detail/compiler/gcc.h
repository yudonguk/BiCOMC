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

#define BICOMC_GCC_VERSION_OR_HIGHER(MAJOR, MINOR) \
	((__GNUC__ == MAJOR && __GNUC_MINOR__ >= MINOR) || (__GNUC__ > MAJOR))

#if defined(__GXX_EXPERIMENTAL_CXX0X__) || (__cplusplus >= 201103L)
#	define BICOMC_IS_GCC_CXX_11_ENABLED (1)
#else
#	define BICOMC_IS_GCC_CXX_11_ENABLED (0)
#endif

#if !defined(BICOMC_NO_NULLPTR)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 6) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_NULLPTR
#	endif
#endif

#if !defined(BICOMC_NO_STATIC_ASSERT)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_STATIC_ASSERT
#	endif
#endif

#if !defined(BICOMC_NO_THREAD_SAFE_STATIC_INIT)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_THREAD_SAFE_STATIC_INIT
#	endif
#endif

#if !defined(BICOMC_NO_CONSTEXPR)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 6) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_CONSTEXPR
#	endif
#endif

#if !defined(BICOMC_NO_NOEXCEPT)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 6) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_NOEXCEPT
#	endif
#endif

#if !defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 4) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_EXPLICIT_DEFAULT_DELETE
#	endif
#endif

#if !defined(BICOMC_NO_RVALUE_REFERENCE)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_RVALUE_REFERENCE
#	endif
#endif

#if !defined(BICOMC_NO_VARIADIC_TEMPLATE)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_VARIADIC_TEMPLATE
#	endif
#endif

#if !defined(BICOMC_NO_CHAR_16_32_TYPE)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 4) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_CHAR_16_32_TYPE
#	endif
#endif

#if !defined(BICOMC_NO_UNICODE_STRING_LITERAL)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 5) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_UNICODE_STRING_LITERAL
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_CSTDINT)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_HEADER_CSTDINT
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_TYPE_TRAITS)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_HEADER_TYPE_TRAITS
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_CODECVT)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(5, 0) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_HEADER_CODECVT
#	endif
#endif

#if !defined(BICOMC_NO_HEADER_ARRAY)
#	if !(BICOMC_GCC_VERSION_OR_HIGHER(4, 3) && BICOMC_IS_GCC_CXX_11_ENABLED)
#		define BICOMC_NO_HEADER_ARRAY
#	endif
#endif

#undef BICOMC_IS_GCC_CXX_11_ENABLED
#undef BICOMC_GCC_VERSION_OR_HIGHER

#endif // !def BICOMC_DETAIL_COMPILER_GCC_H__
