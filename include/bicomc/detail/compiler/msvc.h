#ifndef BICOMC_DETAIL_COMPILER_MSVC_H__
#define BICOMC_DETAIL_COMPILER_MSVC_H__

#if !defined(_MSC_VER)
#	error "compiler is not msvc"
#endif // !def _MSC_VER

#if defined(_WIN32) && !defined(_WIN64) && defined(_M_IX86)
#	define BICOMC_CALL __cdecl
#	define BICOMC_EXPORT __declspec(dllexport)
#elif defined(_WIN32) && !defined(_WIN64) && defined(_M_ARM)
#	error "platform is not supported"
#elif defined(_WIN32) && !defined(_WIN64) && defined(_M_ARMT)
#	error "platform is not supported"
#elif defined(_WIN64) && defined(_M_X64)
#	define BICOMC_CALL
#	define BICOMC_EXPORT __declspec(dllexport)
#elif defined(_WIN64) && defined(_M_IA64)
#	error "platform is not supported"
#else
#	error "platform is not supported"
#endif

#define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
	(_MSC_VER >= 1600)
#define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
	(_MSC_VER >= 1600)
#define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
	(_MSC_VER >= 1900)
#define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
	(_MSC_VER >= 1900)
#define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
	(_MSC_VER >= 1900)
#define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
	(_MSC_VER >= 1800)
#define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
	(_MSC_VER >= 1600)
#define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
	(_MSC_VER > 1800)
#define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
	(_MSC_VER >= 1900)
#define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
	(_MSC_VER >= 1600)
#define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER	\
	(_MSC_VER >= 1600)
#define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
	(_MSC_VER >= 1700)
#define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
	(_MSC_VER >= 1700)
#define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
	(__cplusplus >= 201103L)
#define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
	(_MSC_VER >= 1600)

#endif // !def BICOMC_DETAIL_COMPILER_MSVC_H__
