#ifndef BICOMC_DETAIL_COMPILER_BICOMC_CALL_H__
#define BICOMC_DETAIL_COMPILER_BICOMC_CALL_H__

#if defined(_WIN32) && !defined(_WIN64) && defined(_M_IX86) /* Windows on x86 */
#	if defined(_MSC_VER) /* MSVC */
#		define BICOMC_CALL __stdcall
#	elif defined(__GNUC__) /* GCC */
#		define BICOMC_CALL __attribute__((stdcall))
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL __stdcall
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL __stdcall
#	elif defined(__BORLANDC__) /* borland */
#		define BICOMC_CALL __stdcall
#	endif


#elif defined(_WIN32) && defined(_WIN64) && (defined(_M_AMD64) || defined(_M_X64)) /* Windows on x64 */
#	if defined(_MSC_VER) /* MSVC */
#		define BICOMC_CALL __stdcall
#	elif defined(__GNUC__) /* GCC */
#		define BICOMC_CALL __attribute__((stdcall))
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL __stdcall
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL __stdcall
#	elif defined(__BORLANDC__) /* borland */
#		define BICOMC_CALL __stdcall
#	endif


#elif defined(_WIN32) && !defined(_WIN64) && defined(_M_ARM) /* Windows on ARM */
#	if defined(_MSC_VER) /* MSVC */
#		define BICOMC_CALL
#	elif defined(__GNUC__) /* GCC */
#		define BICOMC_CALL
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL
#	endif


#elif defined(_WIN32) && defined(_WIN64) && defined(_M_ARM64) /* Windows on ARM64 */
#	if defined(_MSC_VER) /* MSVC */
#		define BICOMC_CALL
#	elif defined(__GNUC__) /* GCC */
#		define BICOMC_CALL
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL
#	endif


#elif defined(_WIN32) && defined(_WIN64) && defined(_M_IA64) /* Windows on IA-64 */


#elif defined(__linux__) && defined(__i386__) /* linux on x86 */
#	if defined(__GNUC__) /* GCC */
#		define BICOMC_CALL
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL
#	endif


#elif defined(__linux__) && defined(__x86_64__) /* linux on x64 */
#	if defined(__GNUC__) /* GCC */
#		define BICOMC_CALL
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL
#	endif


#elif defined(__linux__) && defined(__arm__) /* linux on ARM */
#	if defined(__GNUC__) /* GCC */
#		define BICOMC_CALL __attribute__((pcs("aapcs")))
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL __attribute__((pcs("aapcs")))
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL __attribute__((pcs("aapcs")))
#	endif


#elif defined(__linux__) && defined(__aarch64__) /* linux on ARM64 */
#	if defined(__GNUC__) /* GCC */
#		define BICOMC_CALL
#	elif defined(__INTEL_COMPILER) /* ICC */
#		define BICOMC_CALL
#	elif defined(__clang__) /* clang */
#		define BICOMC_CALL
#	endif


#endif

#if !defined(BICOMC_CALL)
#	error "compiler is not supported"
#endif // !def BICOMC_CALL

#endif // !def BICOMC_DETAIL_COMPILER_BICOMC_CALL_H__
