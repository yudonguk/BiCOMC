#ifndef BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
#define BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__

#include "config.h"

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
				::pthread_mutexattr_init(&attr);
				::pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
				::pthread_mutex_init(&mutex, &attr);
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
				::pthread_mutex_trylock(&mutex) == 0;
			}

			void unlock()
			{
				::pthread_mutex_unlock(&mutex);
			}

		private:
			::pthread_mutex_t mutex;
		};

		static Mutex& getMutex()
		{
			static Mutex* pMutex;
			static ::pthread_once_t flag = PTHREAD_ONCE_INIT;

			struct Helper
			{
				static void init()
				{
					static Mutex mutex;
					mutex.lock();
					pMutex = &mutex;
					mutex.unlock();
				}
			};

			::pthread_once(&flag, &Helper::init);
			return *pMutex;
		}

		static void lock()
		{
			getMutex().lock();
		}

		static void unlock()
		{
			getMutex().unlock();
		}
	};

#endif

} // namespace bcc
} // namespace detail

#endif // !def BICOMC_DETAIL_SAFE_STATIC_MUTEX_H__
