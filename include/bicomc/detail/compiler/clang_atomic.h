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

#ifndef BICOMC_DETAIL_COMPILER_CLANG_ATOMIC_H__
#define BICOMC_DETAIL_COMPILER_CLANG_ATOMIC_H__

#if !defined(__clang__)
#	error "compiler is not clang"
#endif // !def __clang__

#if defined(_MSC_VER) && defined(_WIN32)
#	include "msvc_atomic.h"
#elif defined(__GNUC__)
#	include "gcc_atomic.h"
#endif

#endif // !def BICOMC_DETAIL_COMPILER_CLANG_ATOMIC_H__
