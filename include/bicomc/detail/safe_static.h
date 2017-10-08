/*
 * Copyright 2016 Kangwon National University
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

#ifndef BICOMC_DETAIL_SAFE_STATIC_H__
#define BICOMC_DETAIL_SAFE_STATIC_H__

#include "config.h"

#include "../memory.h"
#include "../type_traits.h"

#include "safe_static_mutex.h"

#if BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER || BICOMC_THREAD_SAFE_STATIC_MUTEX_IS_AVAILABLE
namespace bcc
{
namespace detail
{
	template<typename T, typename Local = void>
	class SafeStatic
	{
	private:
		struct InitFunctor
		{
			virtual void operator()(T&) = 0;
		};

		struct NoInitializer : InitFunctor
		{
			void operator()(T&) {}
		};

		template<typename Initializer>
		struct DelegateInitializer : InitFunctor
		{
			DelegateInitializer(Initializer& initializer_)
				: initializer(initializer_)
			{}

			void operator()(T& instance)
			{
				initializer(instance);
			}

			Initializer& initializer;
		};

		struct Holder
		{
			Holder(InitFunctor& initializer)
			{
				initializer(instance);
			}

			T instance;
		};

	public:
		static T* get() BICOMC_NOEXCEPT
		{
			return get(nullptr);
		}

		static T* init()
		{
			NoInitializer helper;
			return get(&helper);
		}

		template<typename Initializer>
		static T* init(Initializer initializer)
		{
			DelegateInitializer<Initializer> helper(initializer);
			return get(&helper);
		}

	private:
		static T* get(InitFunctor* pInitializer)
		{
			static T* pInstance = 0;
			if (pInstance || !pInitializer) return pInstance;

#if !BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER
			SafeStaticMutex::Guard guard;
#endif // !BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER
			static Holder holder(*pInitializer);
			return pInstance = &holder.instance;
		}
	};

} // namespace detail
} // namespace bcc

#else
#	include "atomic.h"

namespace bcc
{
namespace detail
{
	template<typename T, typename Local = void>
	class SafeStatic
	{
	private:
		enum {SAFE_STATIC_NONE = 0, SAFE_STATIC_PREPARING = -1};

		struct NoInitializer
		{
			void operator()(T&) {}
		};

	public:
		static T* get() BICOMC_NOEXCEPT
		{
			static T* p = 0;
			if (p) return p;

			for (;;)
			{
				bcc::intptr_t address = atomic_load(&state());

				switch (address)
				{
				case SafeStatic::SAFE_STATIC_NONE: return nullptr;
				case SafeStatic::SAFE_STATIC_PREPARING: break;
				default: return p = reinterpret_cast<T*>(address);
				}
			}
		}

		static T* init()
		{
			NoInitializer helper;
			return init(helper);
		}

		template<typename Initializer>
		static T* init(Initializer initializer)
		{
			for (bcc::intptr_t address = SafeStatic::SAFE_STATIC_NONE
				; !atomic_compare_exchange_strong(&state(), &address, SafeStatic::SAFE_STATIC_PREPARING)
				; address = SafeStatic::SAFE_STATIC_NONE)
			{
				if (T* p = get())
					return p;
			}

			try
			{
				T* p = bcc::addressof(instance());
				initializer(*p);
				atomic_store(&state(), reinterpret_cast<bcc::intptr_t>(p));
				return p;
			}
			catch (...)
			{
				atomic_store(&state(), SafeStatic::SAFE_STATIC_NONE);
				throw;
			}
		}

	private:
		static atomic_intptr_t& state()
		{
			static atomic_intptr_t state = SafeStatic::SAFE_STATIC_NONE;
			return state;
		}

		static T& instance()
		{
			static T instance;
			return instance;
		}
	};

} // namespace detail
} // namespace bcc

#endif // BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER || BICOMC_THREAD_SAFE_STATIC_MUTEX_IS_AVAILABLE

#endif // !def BICOMC_DETAIL_SAFE_STATIC_H__
