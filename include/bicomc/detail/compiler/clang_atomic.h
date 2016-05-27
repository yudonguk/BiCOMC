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
