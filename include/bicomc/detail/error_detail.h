#ifndef BICOMC_DETAIL_ERROR_DETAIL_H__
#define BICOMC_DETAIL_ERROR_DETAIL_H__

#include "config.h"

#include <string>
#include <utility>

#include "error_fwd.h"
#include "string_util.h"

namespace bcc
{
namespace detail
{
	class UnknownError : public ErrorDetail
	{
	public:
		static bcc::uint32_t const VALUE = 1;
		static bcc::uint32_t const CATEGORY = 1;
	
	private:
		BICOMC_OVERRIDE(ErrorDetail)
			BICOMC_OVER_METHOD_CV(destroy, void());
			BICOMC_OVER_METHOD_C(clone, Object*());
			BICOMC_OVER_METHOD_C(value, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(category, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(message, char const*());
		BICOMC_OVERRIDE_END()

	private:
		UnknownError()
			: BICOMC_OVERRIDE_INIT()
		{}

	public:
		~UnknownError()
		{}

	private:
		UnknownError(UnknownError const&) BICOMC_DELETE;
		UnknownError& operator=(UnknownError const&) BICOMC_DELETE;

	public:
		static UnknownError* instance() BICOMC_NOEXCEPT
		{
			static UnknownError error;
			return &error;
		}

	public:
		void destroy() const volatile
		{}

		Object* clone() const
		{
			return const_cast<UnknownError*>(this);
		}

		bcc::uint32_t value() const
		{
			return VALUE;
		}

		bcc::uint32_t category() const
		{
			return CATEGORY;
		}

		char const* message() const
		{
			static std::string raw
				= StringUtil::convertToUtf8(L"unknown error");
			return raw.c_str();
		}
	};

	class RuntimeError : public ErrorDetail
	{
	public:
		static bcc::uint32_t const VALUE = 3;
		static bcc::uint32_t const CATEGORY = 1;

	private:
		BICOMC_OVERRIDE(ErrorDetail)
			BICOMC_OVER_DESTROY();
			BICOMC_OVER_CLONE();
			BICOMC_OVER_METHOD_C(value, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(category, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(message, char const*());
		BICOMC_OVERRIDE_END()

	public:
		RuntimeError(char const* message)
			: BICOMC_OVERRIDE_INIT()
			, mMessage(StringUtil::convertToUtf8(StringUtil::convertToWstring(message)))
		{}

		RuntimeError(std::string const& message)
			: BICOMC_OVERRIDE_INIT()
			, mMessage(StringUtil::convertToUtf8(StringUtil::convertToWstring(message)))
		{}

		RuntimeError(wchar_t const* message)
			: BICOMC_OVERRIDE_INIT()
			, mMessage(StringUtil::convertToUtf8(message))
		{}

		RuntimeError(std::wstring const& message)
			: BICOMC_OVERRIDE_INIT()
			, mMessage(StringUtil::convertToUtf8(message))
		{}

		RuntimeError(RuntimeError const& error)
			: BICOMC_OVERRIDE_INIT(), mMessage(error.mMessage)
		{}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		RuntimeError(RuntimeError&& error)
			: BICOMC_OVERRIDE_INIT(), mMessage(std::move(error.mMessage))
		{}
#endif

		~RuntimeError()
		{}

	public:
		RuntimeError& operator=(RuntimeError const& error)
		{
			if (this == &error) return *this;
			mMessage = error.mMessage;
			return *this;
		}

#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
		RuntimeError& operator=(RuntimeError&& error)
		{
			if (this == &error) return *this;
			mMessage = std::move(error.mMessage);
			return *this;
		}
#endif

	public:
		bcc::uint32_t value() const
		{
			return VALUE;
		}

		bcc::uint32_t category() const
		{
			return CATEGORY;
		}

		char const* message() const
		{
			return mMessage.c_str();
		}

	private:
		std::string mMessage;
	};

	class UncaughtException : public ErrorDetail
	{
	public:
		static bcc::uint32_t const VALUE = 3;
		static bcc::uint32_t const CATEGORY = 1;
	
	private:
		BICOMC_OVERRIDE(ErrorDetail)
			BICOMC_OVER_METHOD_CV(destroy, void());
			BICOMC_OVER_METHOD_C(clone, Object*());
			BICOMC_OVER_METHOD_C(value, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(category, bcc::uint32_t());
			BICOMC_OVER_METHOD_C(message, char const*());
		BICOMC_OVERRIDE_END()

	private:
		UncaughtException()
			: BICOMC_OVERRIDE_INIT()
		{}
				
	public:
		~UncaughtException()
		{}

	private:
		UncaughtException(UncaughtException const&) BICOMC_DELETE;
		UncaughtException& operator=(UncaughtException const&) BICOMC_DELETE;

	public:
		static UncaughtException* instance() BICOMC_NOEXCEPT
		{
			static UncaughtException error;
			return &error;
		}

	public:
		void destroy() const volatile
		{}

		Object* clone() const
		{
			return const_cast<UncaughtException*>(this);
		}

		bcc::uint32_t value() const
		{
			return VALUE;
		}

		bcc::uint32_t category() const
		{
			return CATEGORY;
		}

		char const* message() const
		{
			static std::string raw
				= StringUtil::convertToUtf8(L"uncaught exception");
			return raw.c_str();
		}
	};

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_ERROR_DETAIL_H__
