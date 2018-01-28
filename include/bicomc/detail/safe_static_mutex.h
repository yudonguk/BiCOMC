/*
 * Copyright 2016 Kangwon National University and Contributors
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

#ifndef BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
#define BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__

#include "config.h"

#include <stdexcept>

namespace bcc
{
namespace detail
{
#if defined(_WIN32_WINNT) && (_WIN32_WINNT >= 0x0600)
#	include <Windows.h>

#	define BICOMC_IS_THREAD_SAFE_STATIC_MUTEX_AVAILABLE 1

	class SafeStaticMutex
	{
	private:
		template<typename, typename> friend class SafeStatic;

		struct Guard
		{
			Guard() { SafeStaticMutex::lock(); }
			~Guard() { SafeStaticMutex::unlock(); }
		};

		class Mutex
		{
		public:
			Mutex()
			{
				InitializeCriticalSection(&mutex);
			}

			~Mutex()
			{
				DeleteCriticalSection(&mutex);
			}

		public:
			void lock()
			{
				EnterCriticalSection(&mutex);
			}

			bool try_lock()
			{
				return TryEnterCriticalSection(&mutex) == TRUE;
			}

			void unlock()
			{
				LeaveCriticalSection(&mutex);
			}

		private:
			CRITICAL_SECTION mutex;
		};

		static Mutex& mutex()
		{
			static INIT_ONCE flag = INIT_ONCE_STATIC_INIT;

			struct Helper
			{
				static BOOL WINAPI init(PINIT_ONCE pFlag, PVOID paramter, PVOID* context)
				{
					try
					{
						mutex().lock();
						mutex().unlock();
					}
					catch (...)
					{
						return FALSE;
					}
					return TRUE;
				}

				static Mutex& mutex()
				{
					static Mutex instance;
					return instance;
				}
			};

			if (InitOnceExecuteOnce(&flag, &Helper::init, nullptr, nullptr) == FALSE)
				throw std::runtime_error("can't initialize mutex for SafeStatic.");
			return Helper::mutex();
		}

		static void lock()
		{
			mutex().lock();
		}

		static void unlock()
		{
			mutex().unlock();
		}
	};

#elif defined(__unix__) || (defined(PTHREAD_ONCE_INIT) && defined(PTHREAD_MUTEX_RECURSIVE))
#	include <pthread.h>

#	define BICOMC_IS_THREAD_SAFE_STATIC_MUTEX_AVAILABLE 1

	class SafeStaticMutex
	{
	private:
		template<typename, typename> friend class SafeStatic;

		struct Guard
		{
			Guard() { SafeStaticMutex::lock(); }
			~Guard() { SafeStaticMutex::unlock(); }
		};

		class Mutex
		{
		public:
			Mutex()
			{
				pthread_mutexattr_t attr;
				if (pthread_mutexattr_init(&attr) != 0)
					throw std::runtime_error("can't initialize mutexattr.");
				
				try
				{
					if (pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
						throw std::runtime_error("can't set mutexattr.");
					if (pthread_mutex_init(&mutex, &attr) != 0)
						throw std::runtime_error("can't initialize mutex.");
				}
				catch (...)
				{
					pthread_mutexattr_destroy(&attr);
					throw;
				}

				pthread_mutexattr_destroy(&attr);
			}

			~Mutex()
			{
				pthread_mutex_destroy(&mutex);
			}

		public:
			void lock()
			{
				pthread_mutex_lock(&mutex);
			}

			bool try_lock()
			{
				return pthread_mutex_trylock(&mutex) == 0;
			}

			void unlock()
			{
				pthread_mutex_unlock(&mutex);
			}

		private:
			pthread_mutex_t mutex;
		};

		static Mutex& mutex()
		{
			static pthread_once_t flag = PTHREAD_ONCE_INIT;

			struct Helper
			{
				static void init()
				{
					try
					{
						mutex().lock();
						mutex().unlock();
					}
					catch (...)
					{
						std::terminate();
					}
				}

				static Mutex& mutex()
				{
					static Mutex instance;
					return instance;
				}
			};

			if (pthread_once(&flag, &Helper::init) != 0)
				throw std::runtime_error("can't initialize mutex for SafeStatic.");
			return Helper::mutex();
		}

		static void lock()
		{
			mutex().lock();
		}

		static void unlock()
		{
			mutex().unlock();
		}
	};

#else
#	define BICOMC_IS_THREAD_SAFE_STATIC_MUTEX_AVAILABLE 0


#endif

} // namespace bcc
} // namespace detail

#endif // !def BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
