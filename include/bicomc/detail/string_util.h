#ifndef BICOMC_DETAIL_STRING_UTIL_H__
#define BICOMC_DETAIL_STRING_UTIL_H__

#include "config.h"

#include <cstdlib>
#include <cstring>
#include <string>

#include "../stdint.h"

#if BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
#include <codecvt>
#include <locale>
#endif // BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER

namespace bcc
{
namespace detail
{
	class StringUtil
	{
	public:
		static std::wstring convertToWstring(std::string const& source)
		{
			return convertToWstring(source.c_str(), source.size());
		}

		static std::wstring convertToWstring(char const* source)
		{
			return convertToWstring(source, std::strlen(source));
		}

		static std::wstring convertToWstring(char const* source, size_t size)
		{
			if (size == 0)
				return std::wstring();

			std::wstring result;
			result.resize(size + 1);

#if defined(_MSC_VER) && _MSC_VER >= 1400
			size_t convertedSize = static_cast<size_t>(-1);
			if (mbstowcs_s(&convertedSize, &result[0], result.size(), source, size) != 0)
				return std::wstring();
			convertedSize -= 1;
#else // defined(_MSC_VER) && _MSC_VER >= 1400
			size_t convertedSize = std::mbstowcs(&result[0], source, result.size());
#endif // defined(_MSC_VER) && _MSC_VER >= 1400

			if (convertedSize == static_cast<size_t>(-1))
				return std::wstring();
			result.resize(convertedSize);
			return result;
		}

		static std::string ConvertFromWstring(std::wstring const& source)
		{
			return ConvertFromWstring(source.c_str(), source.size());
		}

		static std::string ConvertFromWstring(wchar_t const* source)
		{
			return ConvertFromWstring(source, std::wcslen(source));
		}

		static std::string ConvertFromWstring(wchar_t const* source, size_t size)
		{
			if (size == 0)
				return std::string();

			std::string result;
			result.resize(size * MB_CUR_MAX + 1);

#if defined(_MSC_VER) && _MSC_VER >= 1400
			size_t convertedSize = static_cast<size_t>(-1);
			if (wcstombs_s(&convertedSize, &result[0], result.size(), source, size) == 0)
				return std::string();
			convertedSize -= 1;
#else // defined(_MSC_VER) && _MSC_VER >= 1400
			size_t convertedSize = std::wcstombs(&result[0], source, result.size());
#endif // defined(_MSC_VER) && _MSC_VER >= 1400

			if (convertedSize == static_cast<size_t>(-1))
				return std::string();
			result.resize(convertedSize);
			return result;
		}

		static std::wstring convertFromUtf8(std::string const& source)
		{
			return convertFromUtf8(source.c_str(), source.size());
		}

		static std::wstring convertFromUtf8(char const* utf8)
		{
			return convertFromUtf8(utf8, std::strlen(utf8));
		}

		static std::wstring convertFromUtf8(char const* utf8, size_t size)
		{
#if BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
			return std::wstring_convert<std::codecvt_utf8<wchar_t> >().from_bytes(utf8, utf8 + size);
#else // BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
			std::wstring result;
			result.reserve(size);
			wchar_t temp = L'\0';
			for (size_t i = 0; i < size;)
			{
				i += convertFromUtf8(temp, utf8 + i);
				result.append(1, temp);
			}
			return result;
#endif // BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
		}

		static std::string convertToUtf8(std::wstring const& source)
		{
			return convertToUtf8(source.c_str(), source.size());
		}

		static std::string convertToUtf8(wchar_t const* source)
		{
			return convertToUtf8(source, std::wcslen(source));
		}

		static std::string convertToUtf8(wchar_t const* source, size_t size)
		{
#if BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
			return std::wstring_convert<std::codecvt_utf8<wchar_t> >().to_bytes(source, source + size);
#else // BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
			std::string result;
			result.reserve(size * 6);
			for (size_t i = 0; i < size; ++i)
			{
				char buff[6];
				result.append(buff, buff + convertToUtf8(buff, source[i]));
			}
			return result;
#endif // BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER
		}

		static size_t convertToUtf8(char output[6], wchar_t ch) BICOMC_NOEXCEPT
		{
			if (ch <= 0x7F)
			{
				output[0] = char(ch);
				return 1;
			}
			else if (ch <= 0x7FF)
			{
				output[0] = 0xC0 | char(ch >> 6);
				output[1] = 0x80 | char(ch & 0x3F);
				return 2;
			}
			else if (ch <= 0xFFFF)
			{
				output[0] = 0xE0 | char(ch >> 12);
				output[1] = 0x80 | char((ch >> 6) & 0x3F);
				output[2] = 0x80 | char(ch & 0x3F);
				return 3;
			}
#if WCHAR_MAX > 0xFFFF
			else if (ch <= 0x1FFFFF)
			{
				output[0] = 0xF0 | char(ch >> 18);
				output[1] = 0x80 | char((ch >> 12) & 0x3F);
				output[2] = 0x80 | char((ch >> 6) & 0x3F);
				output[3] = 0x80 | char(ch & 0x3F);
				return 4;
			}
			else if (ch <= 0x3FFFFFF)
			{
				output[0] = 0xF8 | char(ch >> 24);
				output[1] = 0x80 | char((ch >> 18) & 0x3F);
				output[2] = 0x80 | char((ch >> 12) & 0x3F);
				output[3] = 0x80 | char((ch >> 6) & 0x3F);
				output[4] = 0x80 | char(ch & 0x3F);
				return 5;
			}
			else if (ch <= 0x7FFFFFFF)
			{
				output[0] = 0xFC | char(ch >> 30);
				output[1] = 0x80 | char((ch >> 24) & 0x3F);
				output[2] = 0x80 | char((ch >> 18) & 0x3F);
				output[3] = 0x80 | char((ch >> 12) & 0x3F);
				output[4] = 0x80 | char((ch >> 6) & 0x3F);
				output[5] = 0x80 | char(ch & 0x3F);
				return 6;
			}
#endif
			return 0;
		}

		static size_t convertFromUtf8(wchar_t& output, char const* utf8) BICOMC_NOEXCEPT
		{
			unsigned char ch = *utf8;
			if (ch <= 0x7F)
			{
				output = utf8[0];
				return 1;
			}
			else if ((ch & 0xE0) == 0xC0)
			{
				output = wchar_t(utf8[0] & 0x1F) << 6;
				output |= wchar_t(utf8[1] & 0x3F);
				return 2;
			}
			else if ((ch & 0xF0) == 0xE0)
			{
				output = wchar_t(utf8[0] & 0x0F) << 12;
				output |= wchar_t(utf8[1] & 0x3F) << 6;
				output |= wchar_t(utf8[2] & 0x3F);
				return 3;
			}
#if WCHAR_MAX > 0xFFFF
			else if ((ch & 0xF8) == 0xF0)
			{
				output = wchar_t(utf8[0] & 0x07) << 18;
				output |= wchar_t(utf8[1] & 0x3F) << 12;
				output |= wchar_t(utf8[2] & 0x3F) << 6;
				output |= wchar_t(utf8[3] & 0x3F);
				return 4;
			}
			else if ((ch & 0xFC) == 0xF8)
			{
				output = wchar_t(utf8[0] & 0x03) << 24;
				output |= wchar_t(utf8[1] & 0x3F) << 18;
				output |= wchar_t(utf8[2] & 0x3F) << 12;
				output |= wchar_t(utf8[3] & 0x3F) << 6;
				output |= wchar_t(utf8[4] & 0x3F);
				return 5;
			}
			else if ((ch & 0xFE) == 0xFC)
			{
				output = wchar_t(utf8[0] & 0x01) << 30;
				output |= wchar_t(utf8[1] & 0x3F) << 24;
				output |= wchar_t(utf8[2] & 0x3F) << 18;
				output |= wchar_t(utf8[3] & 0x3F) << 12;
				output |= wchar_t(utf8[4] & 0x3F) << 6;
				output |= wchar_t(utf8[5] & 0x3F);
				return 6;
			}
#endif
			return 0;
		}
	};
} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_STRING_UTIL_H__
