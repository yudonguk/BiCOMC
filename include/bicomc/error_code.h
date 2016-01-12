#ifndef BICOMC_ERROR_CODE_H__
#define BICOMC_ERROR_CODE_H__

#include "detail/config.h"

#include "stdint.h"

#include <string>

namespace bcc
{
namespace detail
{
	class ErrorDetail;
}

	class ErrorCode
	{
	public:
		ErrorCode();

		ErrorCode(std::nullptr_t);

		ErrorCode(detail::ErrorDetail const& detail);

		explicit ErrorCode(detail::ErrorDetail* pDetail);
			
		ErrorCode(ErrorCode const& error);

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		ErrorCode(ErrorCode&& error);
#endif
		
		~ErrorCode();

	public:
		ErrorCode& operator=(ErrorCode const& error);

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		ErrorCode& operator=(ErrorCode&& error);
#endif

	public:
		typedef void(*unspecified_bool_type)();

		operator unspecified_bool_type() const;

		bool operator!() const;

	public:
		bcc::uint32_t value() const;

		bcc::uint32_t category() const;

		std::string message() const;

		void reset(detail::ErrorDetail* pDetail = nullptr);

		detail::ErrorDetail* release();
		
		void swap(ErrorCode& error);
		
	private:
		detail::ErrorDetail* mpDetail;
	};
} // namespace bcc

#if !defined(BICOMC_OBJECT_H__)
#include "detail/error_code.h"
#endif

#endif // !def BICOMC_ERROR_CODE_H__
