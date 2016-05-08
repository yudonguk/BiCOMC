#ifndef BICOMC_STD_INT_H__
#define BICOMC_STD_INT_H__

#include "detail/config.h"

#include <climits>

#if BICOMC_IS_STD_INT_SUPPORT_COMPILER
#include <cstdint>

#else
#ifndef __STDC_CONSTANT_MACROS
#	define __STDC_CONSTANT_MACROS
#endif // !__STDC_CONSTANT_MACROS
#ifndef __STDC_LIMIT_MACROS
#	define __STDC_LIMIT_MACROS
#endif // !__STDC_LIMIT_MACROS
#ifndef __STDC_FORMAT_MACROS
#	define __STDC_FORMAT_MACROS
#endif // !def __STDC_FORMAT_MACROS

#include "detail/pstdint.h"

#endif // BICOMC_IS_STD_INT_SUPPORT_COMPILER

namespace bcc
{
	using ::int_fast8_t;
	using ::int_fast16_t;
	using ::int_fast32_t;
	using ::int_fast64_t;

	using ::int_least8_t;
	using ::int_least16_t;
	using ::int_least32_t;
	using ::int_least64_t;

	using ::intmax_t;

	using ::uint_fast8_t;
	using ::uint_fast16_t;
	using ::uint_fast32_t;
	using ::uint_fast64_t;

	using ::uint_least8_t;
	using ::uint_least16_t;
	using ::uint_least32_t;
	using ::uint_least64_t;

	using ::uintmax_t;

namespace detail
{
	template<size_t size>
	struct FixedSizeInt
	{
		template<bool condition, typename T1, typename T2>
		struct Helper
		{
			typedef T1 type;
		};

		template<typename T1, typename T2>
		struct Helper<false, T1, T2>
		{
			typedef T2 type;
		};
		
		template<typename T, typename Dummy = void>
		struct IsVoid
		{
			static bool const value = false;
		};

		template<typename Dummy>
		struct IsVoid<void, Dummy>
		{
			static bool const value = true;
		};

		typedef typename Helper<
			sizeof(signed char) == size
			, signed char
			, typename Helper<
				sizeof(signed short int) == size
				, signed short int
				, typename Helper<
					sizeof(signed int) == size
					, signed int
					, typename Helper<
						sizeof(signed long int) == size
						, signed long int
						, typename Helper<
#if defined(LLONG_MAX)
							sizeof(signed long long int) == size
							, signed long long int
#else
							false
							, void
#endif // def LLONG_MAX
							, typename Helper<
								sizeof(bcc::intmax_t) == size
								, bcc::intmax_t
								, void
							>::type
						>::type
					>::type
				>::type
			>::type
		>::type signed_type;
		static_assert(!IsVoid<signed_type>::value, "Mathced int type is not exist.");

		typedef typename Helper<
			sizeof(unsigned char) == size
			, unsigned char
			, typename Helper<
				sizeof(unsigned short int) == size
				, unsigned short int
				, typename Helper<
					sizeof(unsigned int) == size
					, unsigned int
					, typename Helper<
						sizeof(unsigned long int) == size
						, unsigned long int
						, typename Helper<
#if defined(ULLONG_MAX)
							sizeof(unsigned long long int) == size
							, unsigned long long int
#else
							false
							, void
#endif // def ULLONG_MAX
							, typename Helper<
								sizeof(bcc::uintmax_t) == size
								, bcc::uintmax_t
								, void
							>::type
						>::type
					>::type
				>::type
			>::type
		>::type unsigned_type;
		static_assert(!IsVoid<unsigned_type>::value, "Mathced int type is not exist.");
	};
}

#if defined(INT8_MAX)
	using ::int8_t;
#else
	typedef detail::FixedSizeInt<1>::signed_type int8_t;
#endif // def INT8_MAX

#if defined(INT16_MAX)
	using ::int16_t;
#else
	typedef detail::FixedSizeInt<2>::signed_type int16_t;
#endif // def INT16_MAX

#if defined(INT32_MAX)
	using ::int32_t;
#else
	typedef detail::FixedSizeInt<4>::signed_type int32_t;
#endif // def INT32_MAX

#if defined(INT64_MAX)
	using ::int64_t;
#else
	typedef detail::FixedSizeInt<8>::signed_type int64_t;
#endif // def INT64_MAX

#if defined(UINT8_MAX)
	using ::uint8_t;
#else
	typedef detail::FixedSizeInt<1>::unsigned_type uint8_t;
#endif // def UINT8_MAX

#if defined(UINT16_MAX)
	using ::uint16_t;
#else
	typedef detail::FixedSizeInt<2>::unsigned_type uint16_t;
#endif // def UINT16_MAX

#if defined(UINT32_MAX)
	using ::uint32_t;
#else
	typedef detail::FixedSizeInt<4>::unsigned_type uint32_t;
#endif // def UINT32_MAX

#if defined(UINT64_MAX)
	using ::uint64_t;
#else
	typedef detail::FixedSizeInt<8>::unsigned_type uint64_t;
#endif // def UINT64_MAX

#if defined(INTPTR_MAX)
	using ::intptr_t;
#else
	typedef detail::FixedSizeInt<sizeof(void*)>::signed_type intptr_t;
#endif // def INTPTR_MAX

#if defined(UINTPTR_MAX)
	using ::uintptr_t;
#else	
	typedef detail::FixedSizeInt<sizeof(void*)>::unsigned_type uintptr_t;
#endif // def UINTPTR_MAX
}

#endif // !def BICOMC_STD_INT_H__
