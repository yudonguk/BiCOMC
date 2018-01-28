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

#ifndef BICOMC_DETAIL_COMPILER_MSVC_H__
#define BICOMC_DETAIL_COMPILER_MSVC_H__

#if !defined(_MSC_VER)
#	error "compiler is not msvc"
#endif // !def _MSC_VER

#pragma warning(disable:4503)

#if !defined(BICOMC_EXPORT)
#	define BICOMC_EXPORT __declspec(dllexport)
#endif // !def BICOMC_EXPORT

#if !defined(BICOMC_NO_STATIC_ASSERT) && (_MSC_VER < 1600)
#	define BICOMC_NO_NULLPTR
#endif

#if !defined(BICOMC_NO_STATIC_ASSERT) && (_MSC_VER < 1600)
#	define BICOMC_NO_STATIC_ASSERT
#endif

#if !defined(BICOMC_NO_THREAD_SAFE_STATIC_INIT) && (_MSC_VER < 1900)
#	define BICOMC_NO_THREAD_SAFE_STATIC_INIT
#endif

#if !defined(BICOMC_NO_CONSTEXPR) && (_MSC_VER < 1900)
#	define BICOMC_NO_CONSTEXPR
#endif

#if !defined(BICOMC_NO_NOEXCEPT) && (_MSC_VER < 1900)
#	define BICOMC_NO_NOEXCEPT
#endif

#if !defined(BICOMC_NO_EXPLICIT_DEFAULT_DELETE) && (_MSC_VER < 1800)
#	define BICOMC_NO_EXPLICIT_DEFAULT_DELETE
#endif

#if !defined(BICOMC_NO_RVALUE_REFERENCE) && (_MSC_VER < 1600)
#	define BICOMC_NO_RVALUE_REFERENCE
#endif

#if !defined(BICOMC_NO_VARIADIC_TEMPLATE) && (_MSC_VER < 1800)
#	define BICOMC_NO_VARIADIC_TEMPLATE
#endif

#if !defined(BICOMC_NO_CHAR_16_32_TYPE) && (_MSC_VER < 1900)
#	define BICOMC_NO_CHAR_16_32_TYPE
#endif

#if !defined(BICOMC_NO_UNICODE_STRING_LITERAL) && (_MSC_VER < 1900)
#	define BICOMC_NO_UNICODE_STRING_LITERAL
#endif

#if !defined(BICOMC_NO_HEADER_CSTDINT) && (_MSC_VER < 1600)
#	define BICOMC_NO_HEADER_CSTDINT
#endif

#if !defined(BICOMC_NO_HEADER_TYPE_TRAITS) && (_MSC_VER < 1600)
#	define BICOMC_NO_HEADER_TYPE_TRAITS
#endif

#if !defined(BICOMC_NO_HEADER_CODECVT) && (_MSC_VER < 1700)
#	define BICOMC_NO_HEADER_CODECVT
#endif

#if !defined(BICOMC_IS_ARRAY_SUPPORTED) && (_MSC_VER < 1600)
#	define BICOMC_NO_HEADER_ARRAY
#endif

#endif // !def BICOMC_DETAIL_COMPILER_MSVC_H__
