#ifndef BICOMC_DETAIL_COMPILER_INTEL_ATOMIC_H__
#define BICOMC_DETAIL_COMPILER_INTEL_ATOMIC_H__

#if !defined(__INTEL_COMPILER)
#	error "compiler is not intel"
#endif // !def __INTEL_COMPILER

#if defined(_WIN32)
#	include "msvc_atomic.h"
#elif defined(__linux__)
#	include "gcc_atomic.h"
#else
#	error "platform is not supported"
#endif // def _WIN32

#endif // !def BICOMC_DETAIL_COMPILER_INTEL_ATOMIC_H__
