#ifndef BICOMC_DETAIL_COMPILER_GCC_H__
#define BICOMC_DETAIL_COMPILER_GCC_H__

#if !defined(__GNUC__)
#	error "compiler is not gcc"
#endif // !defined(__GNUC__)

#if defined(_WIN32) && !defined(_WIN64) && defined(__i386__)
#	define BICOMC_CALL __attribute__((cdecl))
#	define BICOMC_EXPORT __attribute__((visibility("default")))
#elif defined(_WIN64) && defined(__x86_64__)
#	define BICOMC_CALL
#	define BICOMC_EXPORT __attribute__((visibility("default")))
#elif defined(__linux__) && defined(__i386__)
#	define BICOMC_CALL __attribute__((cdecl))
#	define BICOMC_EXPORT __attribute__((visibility("default")))
#elif defined(__linux__) && defined(__x86_64__)
#	error "platform is not supported"
#elif defined(__linux__) && defined(__arm__)
#	error "platform is not supported"
#elif defined(__linux__) && defined(__thumb__)
#	error "platform is not supported"
#elif defined(__linux__) && defined(__aarch64__)
#	error "platform is not supported"
#else
#	error "platform is not supported"
#endif // defined(_WIN32) && !defined(_WIN64) && defined(__i386__)

#define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER	\
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
	((__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__)) && __GNUC__ > 4)
#define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
	(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))

#endif // !def BICOMC_DETAIL_COMPILER_GCC_H__
