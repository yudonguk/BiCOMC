#ifndef BICOMC_MEMORY_H__
#define BICOMC_MEMORY_H__

#include "detail/config.h"

namespace bcc
{
	template<typename T>
	T* addressof(T& o) BICOMC_NOEXCEPT
	{
		return reinterpret_cast<T*>(&const_cast<char&>(reinterpret_cast<char const volatile&>(o)));
	}
} // namespace bcc

#endif // !def BICOMC_MEMORY_H__
