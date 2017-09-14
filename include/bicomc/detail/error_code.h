#ifndef BICOMC_DETAIL_ERROR_CODE_H__
#define BICOMC_DETAIL_ERROR_CODE_H__

#include "config.h"

#include "../object.h"

#include "error_fwd.h"

#include <string>
#include <utility>

namespace bcc
{
	inline ErrorCode::ErrorCode() BICOMC_NOEXCEPT
		: mpDetail()
	{}

	inline ErrorCode::ErrorCode(std::nullptr_t) BICOMC_NOEXCEPT
		: mpDetail()
	{}

	inline ErrorCode::ErrorCode(detail::ErrorDetail const& detail)
		: mpDetail(bicomc_cast<detail::ErrorDetail*>(detail.clone()))
	{}

	inline ErrorCode::ErrorCode(detail::ErrorDetail* pDetail) BICOMC_NOEXCEPT
		: mpDetail(pDetail)
	{}

	inline ErrorCode::ErrorCode(ErrorCode const& error)
		: mpDetail(error.mpDetail
			? bicomc_cast<detail::ErrorDetail*>(error.mpDetail->clone())
			: nullptr)
	{}

	inline ErrorCode::~ErrorCode() BICOMC_NOEXCEPT
	{
		try
		{
			if (mpDetail)
				mpDetail->destroy();
		}
		catch (...)
		{}
	}

	inline ErrorCode& ErrorCode::operator=(ErrorCode const& error)
	{
		if (this == &error) return *this;
		ErrorCode temp(error);
		swap(temp);
		return *this;
	}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
	inline ErrorCode::ErrorCode(ErrorCode&& error) BICOMC_NOEXCEPT
		: mpDetail()
	{
		swap(error);
	}

	inline ErrorCode& ErrorCode::operator=(ErrorCode&& error) BICOMC_NOEXCEPT
	{
		swap(error);
		return *this;
	}
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER

	inline ErrorCode::operator ErrorCode::unspecified_bool_type() const BICOMC_NOEXCEPT
	{
		struct bool_true_helper { static void bool_true() {} };
		return !mpDetail ? nullptr : &bool_true_helper::bool_true;
	}

	inline bool ErrorCode::operator!() const BICOMC_NOEXCEPT
	{
		return !mpDetail;
	}
	
	inline bcc::uint32_t ErrorCode::value() const BICOMC_NOEXCEPT
	{
		try
		{
			return mpDetail ? mpDetail->value() : 0;
		}
		catch (...)
		{
			return ErrorCode::UNKNOWN;
		}
	}

	inline bcc::uint32_t ErrorCode::category() const BICOMC_NOEXCEPT
	{
		try
		{
			return mpDetail ? mpDetail->category() : 0;
		}
		catch (...)
		{
			return ErrorCode::UNKNOWN;
		}
	}

	inline std::string ErrorCode::message() const
	{
		return mpDetail ? std::string(mpDetail->message()) : std::string();
	}

	inline char const* ErrorCode::what() const BICOMC_NOEXCEPT
	{
		try
		{
			return mpDetail ? mpDetail->message() : "";
		}
		catch (...)
		{
			return "";
		}
	}

	inline void ErrorCode::reset(detail::ErrorDetail* pDetail)
	{
		detail::ErrorDetail* pTemp = mpDetail;
		mpDetail = pDetail;

		if (pTemp) pTemp->destroy();
	}

	inline detail::ErrorDetail* ErrorCode::release() BICOMC_NOEXCEPT
	{
		detail::ErrorDetail* pDetail = mpDetail;
		mpDetail = nullptr;
		return pDetail;
	}

	inline void ErrorCode::swap(ErrorCode& error) BICOMC_NOEXCEPT
	{
		if (this == &error) return;
		std::swap(mpDetail, error.mpDetail);
	}
} // namespace bcc

#endif // !def BICOMC_DETAIL_ERROR_CODE_H__
