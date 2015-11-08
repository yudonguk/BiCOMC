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

	template<typename T>
	struct Compatibility
	{
		static_assert(bcc::is_arithmetic<T>::value
			|| bcc::is_void<T>::value
			|| bcc::is_interface<T>::value
			, "'T' is not compatible type. Please specialize 'bcc::detail::Compatibility'.");

		typedef T type;
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
} // namepsace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_COMPATIBILITY_H__
