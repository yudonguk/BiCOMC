/*
 * Copyright 2015 Kangwon National University
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
