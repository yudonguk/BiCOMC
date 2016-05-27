#ifndef BICOMC_DETAIL_COMPILER_COMPILER_H__
#define BICOMC_DETAIL_COMPILER_COMPILER_H__

#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#	include "msvc.h"

#elif defined(__GNUC__) && !defined(__INTEL_COMPILER)
#	include "gcc.h"

#elif defined(__INTEL_COMPILER)
#	include "intel.h"

#endif

#endif // !def BICOMC_DETAIL_COMPILER_COMPILER_H__
