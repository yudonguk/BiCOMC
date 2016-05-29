#ifndef BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
#define BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__

#include "config.h"

#include <stdexcept>

namespace bcc
{
namespace detail
{
#if defined(PTHREAD_ONCE_INIT) && defined(PTHREAD_MUTEX_RECURSIVE)

#	define BICOMC_THREAD_SAFE_STATIC_MUTEX_IS_AVAILABLE 1

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
				::pthread_mutexattr_t attr;
				if (::pthread_mutexattr_init(&attr) != 0)
					throw std::runtime_error("can't initialize mutexattr.");
				
				try
				{
					if (::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE) != 0)
						throw std::runtime_error("can't set mutexattr.");
					if (::pthread_mutex_init(&mutex, &attr) != 0)
						throw std::runtime_error("can't initialize mutex.");
				}
				catch (...)
				{
					::pthread_mutexattr_destroy(&attr);
					throw;
				}

				::pthread_mutexattr_destroy(&attr);
			}

			~Mutex()
			{
				::pthread_mutex_destroy(&mutex);
			}

		public:
			void lock()
			{
				::pthread_mutex_lock(&mutex);
			}

			bool try_lock()
			{
				return ::pthread_mutex_trylock(&mutex) == 0;
			}

			void unlock()
			{
				::pthread_mutex_unlock(&mutex);
			}

		private:
			::pthread_mutex_t mutex;
		};

		static Mutex& mutex()
		{
			static ::pthread_once_t flag = PTHREAD_ONCE_INIT;

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

			if (::pthread_once(&flag, &Helper::init) != 0)
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

#elif defined(_WINDOWS_)

#	define BICOMC_THREAD_SAFE_STATIC_MUTEX_IS_AVAILABLE 1

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
				::InitializeCriticalSection(&mutex);
			}

			~Mutex()
			{
				::DeleteCriticalSection(&mutex);
			}

		public:
			void lock()
			{
				::EnterCriticalSection(&mutex);
			}

			bool try_lock()
			{
				return ::TryEnterCriticalSection(&mutex) == TRUE;
			}

			void unlock()
			{
				::LeaveCriticalSection(&mutex);
			}

		private:
			::CRITICAL_SECTION mutex;
		};

		static Mutex& mutex()
		{
			static ::INIT_ONCE flag = INIT_ONCE_STATIC_INIT;

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

			if (::InitOnceExecuteOnce(&flag, &Helper::init, nullptr, nullptr) == FALSE)
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

#endif

} // namespace bcc
} // namespace detail

#endif // !def BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
