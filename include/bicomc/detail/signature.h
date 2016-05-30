#ifndef BICOMC_DETAIL_SIGNATURE_H__
#define BICOMC_DETAIL_SIGNATURE_H__

#include "config.h"

#include <sstream>

#include "../stdint.h"
#include "../type_traits.h"

#include "compatibility.h"
#include "method_traits.h"
#include "string_util.h"

#define BICOMC_SIGNATURE_DEFAULT_NAME \
	BiCOMC_Signature_Default__

#define BICOMC_SIGNATURE_CUSTOM_NAME \
	BiCOMC_Signature_Custom__

template<typename T>
struct BICOMC_SIGNATURE_CUSTOM_NAME;
//	static std::wstring to_wstring();
//	static std::string to_utf8();

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
#	define BICOMC_SIGNATURE_DEFAULT(FULL_TYPE_NAME) \
		struct BICOMC_SIGNATURE_DEFAULT_NAME \
		{ \
			static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(FULL_TYPE_NAME); } \
			static std::string to_utf8() { return BICOMC_STRINGIZER(FULL_TYPE_NAME); } \
		};

#	define BICOMC_SIGNATURE(FULL_TYPE_NAME) \
		template<> struct BICOMC_SIGNATURE_CUSTOM_NAME<FULL_TYPE_NAME > \
		{ \
			static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(FULL_TYPE_NAME); } \
			static std::string to_utf8() { return BICOMC_STRINGIZER(FULL_TYPE_NAME); } \
		};

#else
#	define BICOMC_SIGNATURE_DEFAULT(FULL_TYPE_NAME) \
		struct BICOMC_SIGNATURE_DEFAULT_NAME \
		{ \
			static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(FULL_TYPE_NAME); } \
			static std::string to_utf8() { return bcc::detail::StringUtil::convertToUtf8(BICOMC_WSTRINGIZER(FULL_TYPE_NAME)); } \
		};

#	define BICOMC_SIGNATURE(FULL_TYPE_NAME) \
		template<> struct BICOMC_SIGNATURE_CUSTOM_NAME<FULL_TYPE_NAME > \
		{ \
			static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(FULL_TYPE_NAME); } \
			static std::string to_utf8() { return bcc::detail::StringUtil::convertToUtf8(BICOMC_WSTRINGIZER(FULL_TYPE_NAME)); } \
		};

#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

namespace bcc
{
	template<typename T>
	struct is_interface;

namespace detail
{
	template<typename T>
	struct Compatibility;

	template<typename T>
	struct Signature
	{
		static std::wstring to_wstring()
		{
			return to_wstring_impl<T>();
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			return to_utf8_impl<T>();
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_void<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			return L"void";
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			bcc::is_void<U>::value
			, std::string
		>::type to_utf8_impl()
		{
			return u8"void";
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_same<U, bool>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			switch (sizeof(U))
			{
			case 1: return L"bool8_t";
			case 2: return L"bool16_t";
			case 4: return L"bool32_t";
			case 8: return L"bool64_t";
			case 16: return L"bool128_t";
			default:
				{
					std::wstringstream stream;
					stream << L"char" << (sizeof(U) * CHAR_BIT) << L"_t";
					return stream.str();
				}
			}
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			bcc::is_same<U, bool>::value
			, std::string
		>::type to_utf8_impl()
		{
			switch (sizeof(U))
			{
			case 1: return u8"bool8_t";
			case 2: return u8"bool16_t";
			case 4: return u8"bool32_t";
			case 8: return u8"bool64_t";
			case 16: return u8"bool128_t";
			default:
				{
					std::stringstream stream;
					stream << u8"char" << (sizeof(U) * CHAR_BIT) << u8"_t";
					return stream.str();
				}
			}
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		struct IsCharSignature
			: public bcc::integral_constant<bool
				, bcc::is_character<U>::value
				&& !bcc::is_same<U, signed char>::value
				&& !bcc::is_same<U, unsigned char>::value>
		{};

		template<typename U>
		static typename bcc::enable_if<
			IsCharSignature<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			switch (sizeof(U))
			{
			case 1: return L"char8_t";
			case 2: return L"char16_t";
			case 4: return L"char32_t";
			case 8: return L"char64_t";
			case 16: return L"char128_t";
			default:
				{
					std::wstringstream stream;
					stream << L"char" << (sizeof(U) * CHAR_BIT) << L"_t";
					return stream.str();
				}
			}
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			IsCharSignature<U>::value
			, std::string
		>::type to_utf8_impl()
		{
			switch (sizeof(U))
			{
			case 1: return u8"char8_t";
			case 2: return u8"char16_t";
			case 4: return u8"char32_t";
			case 8: return u8"char64_t";
			case 16: return u8"char128_t";
			default:
				{
					std::stringstream stream;
					stream << u8"char" << (sizeof(U) * CHAR_BIT) << u8"_t";
					return stream.str();
				}
			}
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_integral<U>::value
			&& bcc::is_signed<U>::value
			&& !IsCharSignature<U>::value
			&& !bcc::is_same<U, bool>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			switch (sizeof(U))
			{
			case 1: return L"int8_t";
			case 2: return L"int16_t";
			case 4: return L"int32_t";
			case 8: return L"int64_t";
			case 16: return L"int128_t";
			default:
				{
					std::wstringstream stream;
					stream << L"int" << (sizeof(U) * CHAR_BIT) << L"_t";
					return stream.str();
				}
			}
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			bcc::is_integral<U>::value
			&& bcc::is_signed<U>::value
			&& !IsCharSignature<U>::value
			&& !bcc::is_same<U, bool>::value
			, std::string
		>::type to_utf8_impl()
		{
			switch (sizeof(U))
			{
			case 1: return u8"int8_t";
			case 2: return u8"int16_t";
			case 4: return u8"int32_t";
			case 8: return u8"int64_t";
			case 16: return u8"int128_t";
			default:
				{
					std::stringstream stream;
					stream << u8"int" << (sizeof(U) * CHAR_BIT) << u8"_t";
					return stream.str();
				}
			}
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_integral<U>::value
			&& !bcc::is_signed<U>::value
			&& !IsCharSignature<U>::value
			&& !bcc::is_same<U, bool>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			switch (sizeof(U))
			{
			case 1: return L"uint8_t";
			case 2: return L"uint16_t";
			case 4: return L"uint32_t";
			case 8: return L"uint64_t";
			case 16: return L"uint128_t";
			default:
				{
					std::wstringstream stream;
					stream << L"uint" << (sizeof(U) * CHAR_BIT) << L"_t";
					return stream.str();
				}
			}
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			bcc::is_integral<U>::value
			&& !bcc::is_signed<U>::value
			&& !IsCharSignature<U>::value
			&& !bcc::is_same<U, bool>::value
			, std::string
		>::type to_utf8_impl()
		{
			switch (sizeof(U))
			{
			case 1: return u8"uint8_t";
			case 2: return u8"uint16_t";
			case 4: return u8"uint32_t";
			case 8: return u8"uint64_t";
			case 16: return u8"uint128_t";
			default:
				{
					std::stringstream stream;
					stream << u8"uint" << (sizeof(U) * CHAR_BIT) << u8"_t";
					return stream.str();
				}
			}
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_floating_point<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			switch (sizeof(U))
			{
			case 1: return L"float8_t";
			case 2: return L"float16_t";
			case 4: return L"float32_t";
			case 8: return L"float64_t";
			case 10: return L"float80_t";
			case 16: return L"float128_t";
			default:
				{
					std::wstringstream stream;
					stream << L"float" << (sizeof(U) * CHAR_BIT) << L"_t";
					return stream.str();
				}
			}
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			bcc::is_floating_point<U>::value
			, std::string
		>::type to_utf8_impl()
		{
			switch (sizeof(U))
			{
			case 1: return u8"float8_t";
			case 2: return u8"float16_t";
			case 4: return u8"float32_t";
			case 8: return u8"float64_t";
			case 10: return u8"float80_t";
			case 16: return u8"float128_t";
			default:
				{
					std::stringstream stream;
					stream << u8"float" << (sizeof(U) * CHAR_BIT) << u8"_t";
					return stream.str();
				}
			}
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER

		template<typename U>
		struct HasTypeSignature
		{
			typedef bcc::int8_t TrueType;
			typedef bcc::int16_t FalseType;

			template <typename C, C> struct Check;

			template<typename C> static TrueType test_to_wstring(Check<std::wstring(*)(), &C::to_wstring>*);
			template<typename C> static FalseType test_to_wstring(...);

			template<typename C> static TrueType test_to_utf8(Check<std::string(*)(), &C::to_utf8>*);
			template<typename C> static FalseType test_to_utf8(...);

			template<typename C> static TrueType test_default(typename C::BICOMC_SIGNATURE_DEFAULT_NAME*);
			template<typename C> static FalseType test_default(...);

			static bool const has_default = sizeof(test_default<U>(0)) == sizeof(TrueType);
			static bool const has_to_wstring = sizeof(test_to_wstring<BICOMC_SIGNATURE_CUSTOM_NAME<U> >(0)) == sizeof(TrueType);
			static bool const has_to_utf8 = sizeof(test_to_utf8<BICOMC_SIGNATURE_CUSTOM_NAME<U> >(0)) == sizeof(TrueType);
		};

		template<typename U>
		static typename bcc::enable_if<
			!bcc::is_fundamental<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			typedef typename bcc::conditional<
				HasTypeSignature<U>::has_to_wstring
				, bcc::true_type, bcc::false_type
			>::type Decider;

			return to_wstring_from_custom_signature<U>(Decider());
		}

		template<typename U>
		static std::wstring to_wstring_from_custom_signature(bcc::false_type)
		{
			static_assert(HasTypeSignature<U>::has_default
				, "Please use 'BICOMC_SIGNATURE()' or specialize 'bcc::detail::Signature'.");

			typedef typename U::BICOMC_SIGNATURE_DEFAULT_NAME DefaultSignature;
			return DefaultSignature::to_wstring();
		}

		template<typename U>
		static std::wstring to_wstring_from_custom_signature(bcc::true_type)
		{
			return BICOMC_SIGNATURE_CUSTOM_NAME<U>::to_wstring();
		}

#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		template<typename U>
		static typename bcc::enable_if<
			!bcc::is_fundamental<U>::value
			, std::string
		>::type to_utf8_impl()
		{
			typedef typename bcc::conditional<
				HasTypeSignature<U>::has_to_utf8
				, bcc::true_type, bcc::false_type
			>::type Decider;

			return to_utf8_from_custom_signature<U>(Decider());
		}

		template<typename U>
		static std::string to_utf8_from_custom_signature(bcc::false_type)
		{
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}

		template<typename U>
		static std::string to_utf8_from_custom_signature(bcc::true_type)
		{
			return BICOMC_SIGNATURE_CUSTOM_NAME<U>::to_utf8();
		}
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
	};

	template<typename T>
	struct Signature<T const>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L" const");
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(u8" const");
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};

	template<typename T>
	struct Signature<T volatile>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L" volatile");
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(u8" volatile");
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};

	template<typename T>
	struct Signature<T const volatile>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L" const volatile");
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(u8" const volatile");
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};

	template<typename T>
	struct Signature<T&>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(1, L'&');
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(1, char(38)); // 38 == u8'&'
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct Signature<T&&>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(2, L'&');
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(2, char(38)); // 38 == u'&'
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	template<typename T>
	struct Signature<T*>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(1, L'*');
			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<T>::to_utf8());
			result.append(1, char(42)); // 42 == u8'*'
			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};

	template<typename Function, typename Owner, bool isConst, bool isVolatile>
	struct Signature<bcc::detail::MethodTypeDeducer<Function, Owner, isConst, isVolatile> >
	{
		typedef bcc::detail::MethodTypeDeducer<Function, Owner, isConst, isVolatile> Deducer;
		typedef typename Deducer::trait Trait;
		typedef typename Trait::ret Ret;
		typedef typename Trait::params Params;

		template<typename Tuple, size_t index = bcc::tuple_size<Tuple>::value>
		struct Itor
		{
			static size_t const size = bcc::tuple_size<Tuple>::value;
			typedef typename bcc::tuple_element<size - index, Tuple>::type type;

			static void to_wstring(std::wstring& result)
			{
				to_wstring_impl<index>(result);
				Itor<Tuple, index - 1>::to_wstring(result);
			}

			template<size_t i>
			static typename bcc::enable_if<i == size>::type to_wstring_impl(std::wstring& result)
			{
				result.append(Signature<type>::to_wstring());
			}

			template<size_t i>
			static typename bcc::enable_if<i < size>::type to_wstring_impl(std::wstring& result)
			{
				result.append(1, L',');
				result.append(Signature<type>::to_wstring());
			}

			static void to_utf8(std::string& result)
			{
				to_utf8_impl<index>(result);
				Itor<Tuple, index - 1>::to_utf8(result);
			}

			template<size_t i>
			static typename bcc::enable_if<i == size>::type to_utf8_impl(std::string& result)
			{
				result.append(Signature<type>::to_utf8());
			}

			template<size_t i>
			static typename bcc::enable_if<i < size>::type to_utf8_impl(std::string& result)
			{
				result.append(1, char(44)); // 44 == u8','
				result.append(Signature<type>::to_utf8());
			}
		};

		template<typename Tuple>
		struct Itor<Tuple, 0>
		{
			static void to_wstring(std::wstring& result)
			{}

			static void to_utf8(std::string& result)
			{}
		};

		static std::wstring to_wstring()
		{
			std::wstring result(Signature<Ret>::to_wstring());

			result.append(1, L'(');
			Itor<Params>::to_wstring(result);
			result.append(1, L')');

			if (isConst)
				result.append(L" const");
			if (isVolatile)
				result.append(L" volatile");

			return result;
		}

		static std::string to_utf8()
		{
#if BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
			std::string result(Signature<Ret>::to_utf8());

			result.append(1, char(40)); // 40 == u8'('
			Itor<Params>::to_utf8(result);
			result.append(1, char(41)); // 41 == u8')'

			if (isConst)
				result.append(u8" const");
			if (isVolatile)
				result.append(u8" volatile");

			return result;
#else
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
#endif // BICOMC_IS_UNICODE_STRING_LITERAL_SUPPORT_COMPILER
		}
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_SIGNATURE_H__
