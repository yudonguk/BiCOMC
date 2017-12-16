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

#ifndef BICOMC_DETAIL_COMPILER_INTEL_H__
#define BICOMC_DETAIL_COMPILER_INTEL_H__

#if !defined(__INTEL_COMPILER)
#	error "compiler is not intel"
#endif // !defined(__INTEL_COMPILER)

#if defined(_WIN32)
#	include "msvc.h"
#elif defined(__linux__)
#	include "gcc.h"
#else
#	error "platform is not supported"
#endif // defined(_WIN32)

#endif // !def BICOMC_DETAIL_COMPILER_INTEL_H__
