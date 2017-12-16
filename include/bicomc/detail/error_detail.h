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

#ifndef BICOMC_DETAIL_ERROR_DETAIL_H__
#define BICOMC_DETAIL_ERROR_DETAIL_H__

#include "config.h"

#include <string>
#include <utility>

#include "error_fwd.h"
#include "safe_static.h"
#include "string_util.h"

namespace bcc
{
namespace detail
{
	class UnknownError : public ErrorDetail
	{
	private:
		struct StaticLocal {};
		typedef bcc::detail::SafeStatic<UnknownError, StaticLocal> StaticHolder;

		friend class bcc::detail::SafeStatic<UnknownError, StaticLocal>;

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
			UnknownError* pInstance = StaticHolder::get();
			if (!pInstance) pInstance = StaticHolder::init();
			return pInstance;
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
			typedef bcc::detail::SafeStatic<std::string, StaticLocal> StaticHolder;

			std::string* pMessage = StaticHolder::get();
			if (!pMessage) pMessage = StaticHolder::init(&UnknownError::getMessage);
			return pMessage->c_str();
		}

	private:
		static void getMessage(std::string& message)
		{
			message = StringUtil::convertToUtf8(L"unknown error");
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
	private:
		struct StaticLocal {};
		typedef bcc::detail::SafeStatic<UncaughtException, StaticLocal> StaticHolder;

		friend class bcc::detail::SafeStatic<UncaughtException, StaticLocal>;

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
			UncaughtException* pInstance = StaticHolder::get();
			if (!pInstance) pInstance = StaticHolder::init();
			return pInstance;
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
			typedef bcc::detail::SafeStatic<std::string, StaticLocal> StaticHolder;

			std::string* pMessage = StaticHolder::get();
			if (!pMessage) pMessage = StaticHolder::init(&UncaughtException::getMessage);
			return pMessage->c_str();
		}

	private:
		static void getMessage(std::string& message)
		{
			message = StringUtil::convertToUtf8(L"uncaught exception");
		}
	};

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_ERROR_DETAIL_H__
