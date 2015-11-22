#ifndef BICOMC_DETAIL_METHOD_CALL_H__
#define BICOMC_DETAIL_METHOD_CALL_H__

#include "config.h"

#include <exception>

#include "../stdint.h"
#include "../type_traits.h"

#include "object_helper.h"
#include "method_return.h"

#define BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY try
#define BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH \
	catch (std::exception const& e) \
	{ \
		try { return new bcc::detail::RuntimeError(e.what()); } catch(...) { return bcc::detail::UncaughtException::instance(); } \
	} \
	catch (bcc::detail::ErrorDetail const& e) \
	{ \
		try { return bcc::ErrorCode(e).release(); } catch(...) { return bcc::detail::UncaughtException::instance(); } \
	} \
	catch (bcc::ErrorCode const& e) \
	{ \
		try { return bcc::ErrorCode(e).release(); } catch(...) { return bcc::detail::UncaughtException::instance(); } \
	} \
	catch (...) { return bcc::detail::UnknownError::instance(); }

namespace bcc
{
	class Object;

namespace detail
{
	class ErrorDetail;

	struct ObjectCaster
	{
		template<typename T>
		struct Helper
		{
			typedef typename T::BiCOMC_Multiple_Inheritnace_Checker Checker;
			static bool const value = Checker::value;
		};

		template<typename Origin, typename Interface>
		static typename bcc::enable_if<Helper<Origin>::value, Origin&>::type cast(Interface& impl)
		{
			return *reinterpret_cast<Origin*>(bcc::detail::ObjectHelper::first(impl));
		}

		template<typename Origin, typename Interface>
		static typename bcc::enable_if<!Helper<Origin>::value, Origin&>::type cast(Interface& impl)
		{
			return static_cast<Origin&>(impl);
		}
	};

	template<typename Method, Method>
	struct MethodCallHelper;

#if BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER
	template<typename Owner, typename Ret, typename... Params, Ret(Owner::*function)(Params...)>
	struct MethodCallHelper<Ret(Owner::*)(Params...), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<Params>(params)...));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p...));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename... Params, void(Owner::*function)(Params...)>
	struct MethodCallHelper<void(Owner::*)(Params...), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<Params>(params)...);
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p...);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename... Params, Ret(Owner::*function)(Params...) const>
	struct MethodCallHelper<Ret(Owner::*)(Params...) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<Params>(params)...));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p...));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename... Params, void(Owner::*function)(Params...) const>
	struct MethodCallHelper<void(Owner::*)(Params...) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<Params>(params)...);
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p...);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename... Params, Ret(Owner::*function)(Params...) volatile>
	struct MethodCallHelper<Ret(Owner::*)(Params...) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<Params>(params)...));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile >(impl).*function)(p...));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename... Params, void(Owner::*function)(Params...) volatile>
	struct MethodCallHelper<void(Owner::*)(Params...) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<Params>(params)...);
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p...);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename... Params, Ret(Owner::*function)(Params...) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(Params...) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<Params>(params)...));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile >(impl).*function)(p...));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename... Params, void(Owner::*function)(Params...) const volatile>
	struct MethodCallHelper<void(Owner::*)(Params...) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<Params>(params)...);
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p...);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, Params... params)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

#else // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER

	template<typename Owner, typename Ret, Ret(Owner::*function)()>
	struct MethodCallHelper<Ret(Owner::*)(), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)());
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, void(Owner::*function)()>
	struct MethodCallHelper<void(Owner::*)(), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				(ObjectCaster::template cast<Owner>(impl).*function)();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, Ret(Owner::*function)() const>
	struct MethodCallHelper<Ret(Owner::*)() const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)());
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, void(Owner::*function)() const>
	struct MethodCallHelper<void(Owner::*)() const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				(ObjectCaster::template cast<Owner const>(impl).*function)();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, Ret(Owner::*function)() volatile>
	struct MethodCallHelper<Ret(Owner::*)() volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)());
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, void(Owner::*function)() volatile>
	struct MethodCallHelper<void(Owner::*)() volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				(ObjectCaster::template cast<Owner volatile>(impl).*function)();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, Ret(Owner::*function)() const volatile>
	struct MethodCallHelper<Ret(Owner::*)() const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)());
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, void(Owner::*function)() const volatile>
	struct MethodCallHelper<void(Owner::*)() const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)();
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, Ret(Owner::*function)(P1)>
	struct MethodCallHelper<Ret(Owner::*)(P1), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, void(Owner::*function)(P1)>
	struct MethodCallHelper<void(Owner::*)(P1), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, Ret(Owner::*function)(P1) const>
	struct MethodCallHelper<Ret(Owner::*)(P1) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, void(Owner::*function)(P1) const>
	struct MethodCallHelper<void(Owner::*)(P1) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, Ret(Owner::*function)(P1) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, void(Owner::*function)(P1) volatile>
	struct MethodCallHelper<void(Owner::*)(P1) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, Ret(Owner::*function)(P1) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, void(Owner::*function)(P1) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, Ret(Owner::*function)(P1, P2)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, void(Owner::*function)(P1, P2)>
	struct MethodCallHelper<void(Owner::*)(P1, P2), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, Ret(Owner::*function)(P1, P2) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, void(Owner::*function)(P1, P2) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, Ret(Owner::*function)(P1, P2) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, void(Owner::*function)(P1, P2) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, Ret(Owner::*function)(P1, P2) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, void(Owner::*function)(P1, P2) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, Ret(Owner::*function)(P1, P2, P3)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, void(Owner::*function)(P1, P2, P3)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, Ret(Owner::*function)(P1, P2, P3) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, void(Owner::*function)(P1, P2, P3) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, Ret(Owner::*function)(P1, P2, P3) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, void(Owner::*function)(P1, P2, P3) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, Ret(Owner::*function)(P1, P2, P3) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, void(Owner::*function)(P1, P2, P3) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, Ret(Owner::*function)(P1, P2, P3, P4)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, void(Owner::*function)(P1, P2, P3, P4)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, Ret(Owner::*function)(P1, P2, P3, P4) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, void(Owner::*function)(P1, P2, P3, P4) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, Ret(Owner::*function)(P1, P2, P3, P4) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};
	template<typename Owner, typename P1, typename P2, typename P3, typename P4, void(Owner::*function)(P1, P2, P3, P4) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, Ret(Owner::*function)(P1, P2, P3, P4) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, void(Owner::*function)(P1, P2, P3, P4) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, Ret(Owner::*function)(P1, P2, P3, P4, P5)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, void(Owner::*function)(P1, P2, P3, P4, P5)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, Ret(Owner::*function)(P1, P2, P3, P4, P5) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, void(Owner::*function)(P1, P2, P3, P4, P5) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, Ret(Owner::*function)(P1, P2, P3, P4, P5) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, void(Owner::*function)(P1, P2, P3, P4, P5) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, Ret(Owner::*function)(P1, P2, P3, P4, P5) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, void(Owner::*function)(P1, P2, P3, P4, P5) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, void(Owner::*function)(P1, P2, P3, P4, P5, P6)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, void(Owner::*function)(P1, P2, P3, P4, P5, P6) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, void(Owner::*function)(P1, P2, P3, P4, P5, P6) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, void(Owner::*function)(P1, P2, P3, P4, P5, P6) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};
	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29)>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29), function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29)>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29), function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29));
#else
				(ObjectCaster::template cast<Owner>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29));
#else
				(ObjectCaster::template cast<Owner const>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29));
#else
				(ObjectCaster::template cast<Owner volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename Ret, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, Ret(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const volatile>
	struct MethodCallHelper<Ret(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const volatile, function>
	{
		typedef ReturnHelper<Ret> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29)));
#else
				ret = RH::fromReturn((ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29));
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

	template<typename Owner, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10, typename P11, typename P12, typename P13, typename P14, typename P15, typename P16, typename P17, typename P18, typename P19, typename P20, typename P21, typename P22, typename P23, typename P24, typename P25, typename P26, typename P27, typename P28, typename P29, void(Owner::*function)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const volatile>
	struct MethodCallHelper<void(Owner::*)(P1, P2, P3, P4, P5, P6, P7, P8, P9, P10, P11, P12, P13, P14, P15, P16, P17, P18, P19, P20, P21, P22, P23, P24, P25, P26, P27, P28, P29) const volatile, function>
	{
		typedef ReturnHelper<void> RH;

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL call(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
#if BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(std::forward<P1>(p1), std::forward<P2>(p2), std::forward<P3>(p3), std::forward<P4>(p4), std::forward<P5>(p5), std::forward<P6>(p6), std::forward<P7>(p7), std::forward<P8>(p8), std::forward<P9>(p9), std::forward<P10>(p10), std::forward<P11>(p11), std::forward<P12>(p12), std::forward<P13>(p13), std::forward<P14>(p14), std::forward<P15>(p15), std::forward<P16>(p16), std::forward<P17>(p17), std::forward<P18>(p18), std::forward<P19>(p19), std::forward<P20>(p20), std::forward<P21>(p21), std::forward<P22>(p22), std::forward<P23>(p23), std::forward<P24>(p24), std::forward<P25>(p25), std::forward<P26>(p26), std::forward<P27>(p27), std::forward<P28>(p28), std::forward<P29>(p29));
#else
				(ObjectCaster::template cast<Owner const volatile>(impl).*function)(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10, p11, p12, p13, p14, p15, p16, p17, p18, p19, p20, p21, p22, p23, p24, p25, p26, p27, p28, p29);
#endif // BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER
				return nullptr;
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}

		template<typename Interface>
		static detail::ErrorDetail* BICOMC_CALL callNoOver(Interface const volatile& impl, typename RH::mediator& ret, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10, P11 p11, P12 p12, P13 p13, P14 p14, P15 p15, P16 p16, P17 p17, P18 p18, P19 p19, P20 p20, P21 p21, P22 p22, P23 p23, P24 p24, P25 p25, P26 p26, P27 p27, P28 p28, P29 p29)
		{
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_TRY
			{
				return new bcc::detail::RuntimeError(L"is not overrided.");
			}
			BICOMC_METHOD_CALL_HELPER_EXCEPTION_CATCH;
			return bcc::detail::UnknownError::instance();
		}
	};

#endif // BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER

} // namespace detail
} // namespace bcc

#endif // !def BICOMC_DETAIL_METHOD_CALL_H__
