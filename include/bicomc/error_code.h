#ifndef BICOMC_ERROR_CODE_H__
#define BICOMC_ERROR_CODE_H__

#include "detail/config.h"

#include "stdint.h"

#include <exception>
#include <string>

namespace bcc
{
namespace detail
{
	class ErrorDetail;
}

	class ErrorCode : public std::exception
	{
	public:
		static bcc::uint32_t const UNKNOWN = static_cast<bcc::uint32_t>(-1);

	public:
		ErrorCode() BICOMC_NOEXCEPT;

		ErrorCode(std::nullptr_t) BICOMC_NOEXCEPT;

		ErrorCode(detail::ErrorDetail const& detail);

		explicit ErrorCode(detail::ErrorDetail* pDetail) BICOMC_NOEXCEPT;
			
		ErrorCode(ErrorCode const& error);

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		ErrorCode(ErrorCode&& error) BICOMC_NOEXCEPT;
#endif
		
		~ErrorCode() BICOMC_NOEXCEPT;

	public:
		ErrorCode& operator=(ErrorCode const& error);

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		ErrorCode& operator=(ErrorCode&& error) BICOMC_NOEXCEPT;
#endif

	public:
		typedef void(*unspecified_bool_type)();

		operator unspecified_bool_type() const BICOMC_NOEXCEPT;

		bool operator!() const BICOMC_NOEXCEPT;

	public:
		bcc::uint32_t value() const BICOMC_NOEXCEPT;

		bcc::uint32_t category() const BICOMC_NOEXCEPT;

		std::string message() const;

		char const* what() const BICOMC_NOEXCEPT;

	public:
		void reset(detail::ErrorDetail* pDetail = nullptr);

		detail::ErrorDetail* release() BICOMC_NOEXCEPT;
		
		void swap(ErrorCode& error) BICOMC_NOEXCEPT;
		
	private:
		detail::ErrorDetail* mpDetail;
	};
} // namespace bcc

#if !defined(BICOMC_OBJECT_H__)
#include "detail/error_code.h"
#endif

#endif // !def BICOMC_ERROR_CODE_H__
