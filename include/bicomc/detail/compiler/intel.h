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
