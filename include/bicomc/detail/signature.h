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

#define BICOMC_SIGNATURE(FULL_TYPE_NAME) \
	template<> struct BICOMC_SIGNATURE_CUSTOM_NAME<FULL_TYPE_NAME > \
	{ \
		static std::wstring to_wstring() { return BICOMC_WSTRINGIZER(FULL_TYPE_NAME); } \
	};

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
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}

		template<typename U>
		static typename bcc::enable_if<
			bcc::is_void<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			return L"void";
		}

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

		template<typename U>
		struct HasTypeSignature
		{
			typedef bcc::int8_t TrueType;
			typedef bcc::int16_t FalseType;

			template <typename C, C> struct Check;

			template<typename C> static TrueType test_custom(Check<std::wstring(*)(), &C::to_wstring>*);
			template<typename C> static FalseType test_custom(...);

			template<typename C> static TrueType test_default(typename C::BICOMC_SIGNATURE_DEFAULT_NAME*);
			template<typename C> static FalseType test_default(...);

			static bool const has_default = sizeof(test_default<U>(0)) == sizeof(TrueType);
			static bool const has_custom = sizeof(test_custom<BICOMC_SIGNATURE_CUSTOM_NAME<U> >(0)) == sizeof(TrueType);
		};

		template<typename U>
		static typename bcc::enable_if<
			!bcc::is_fundamental<U>::value
			, std::wstring
		>::type to_wstring_impl()
		{
			typedef typename bcc::conditional<
				HasTypeSignature<U>::has_custom
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
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
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
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
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
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}
	};

	template<typename T>
	struct Signature<T&>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L"&");
			return result;
		}

		static std::string to_utf8()
		{
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}
	};

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	template<typename T>
	struct Signature<T&&>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L"&&");
			return result;
		}

		static std::string to_utf8()
		{
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}
	};
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	template<typename T>
	struct Signature<T*>
	{
		static std::wstring to_wstring()
		{
			std::wstring result(Signature<T>::to_wstring());
			result.append(L"*");
			return result;
		}

		static std::string to_utf8()
		{
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
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
				result.append(L",");
				result.append(Signature<type>::to_wstring());
			}
		};

		template<typename Tuple>
		struct Itor<Tuple, 0>
		{
			static void to_wstring(std::wstring& result)
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
			return bcc::detail::StringUtil::convertToUtf8(to_wstring());
		}
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_SIGNATURE_H__
