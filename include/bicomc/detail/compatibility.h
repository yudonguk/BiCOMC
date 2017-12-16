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

#ifndef BICOMC_DETAIL_COMPATIBILITY_H__
#define BICOMC_DETAIL_COMPATIBILITY_H__

#include "config.h"

#include "../stdint.h"
#include "../type_traits.h"

#include "signature.h"

namespace bcc
{
	class Object;

	template<typename T>
	struct is_interface;

namespace detail
{
	struct ReferenceSizeTester { int& value; };
	static_assert(sizeof(ReferenceSizeTester) == sizeof(int*), "Reference size must equal pointer size.");
	static_assert(sizeof(bcc::uintptr_t) == sizeof(void*), "'bcc::uintptr_t' size must equal pointer size.");
	static_assert(sizeof(bcc::intptr_t) == sizeof(void*), "'bcc::intptr_t' size must equal pointer size.");

	template<typename T, bool isCompatible = bcc::is_arithmetic<T>::value || bcc::is_void<T>::value || bcc::is_interface<T>::value>
	struct CompatibilityHelper
	{
		typedef T type;
	};

	template<typename T>
	struct CompatibilityHelper<T, false>
	{};

	template<typename T>
	struct Compatibility : public CompatibilityHelper<T>
	{
		// 'T' is not compatible type. Please specialize 'bcc::detail::Compatibility'.
		// typedef T type;
	};

	template<typename T>
	struct Compatibility<T const>
	{
		typedef typename Compatibility<T>::type const type;
	};

	template<typename T>
	struct Compatibility<T volatile>
	{
		typedef typename Compatibility<T>::type volatile type;
	};

	template<typename T>
	struct Compatibility<T const volatile>
	{
		typedef typename Compatibility<T>::type const volatile type;
	};

	template<typename T>
	struct Compatibility<T&>
	{
		typedef typename Compatibility<T>::type& type;
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct Compatibility<T&&>
	{
		typedef typename Compatibility<T>::type&& type;
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER 

	template<typename T>
	struct Compatibility<T*>
	{
		typedef typename Compatibility<T>::type* type;
	};

	template<typename T>
	struct has_compatibility_impl
	{
		typedef bcc::int8_t TrueType;
		typedef bcc::int16_t FalseType;

		typedef typename bcc::remove_cv<T>::type type;

		template<typename U> static TrueType test(typename U::type*);
		template<typename U> static FalseType test(...);

		static bool const value = sizeof(test<Compatibility<type> >(0)) == sizeof(TrueType);
	};

	template<typename T>
	struct has_compatibility_impl<T*>
	{
		static bool const value = has_compatibility_impl<T>::value;
	};

	template<typename T>
	struct has_compatibility_impl<T&>
	{
		static bool const value = has_compatibility_impl<T>::value;
	};

} // namepsace detail

	template<typename T>
	struct has_compatibility
		: public integral_constant<bool, detail::has_compatibility_impl<T>::value>
	{};

} // namespace bcc

#endif // !def BICOMC_DETAIL_COMPATIBILITY_H__
