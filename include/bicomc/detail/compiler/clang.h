#ifndef BICOMC_DETAIL_COMPILER_CLANG_H__
#define BICOMC_DETAIL_COMPILER_CLANG_H__

#if !defined(__clang__)
#	error "compiler is not clang"
#endif // !def __clang__

#if defined(_MSC_VER) && defined(_WIN32)
#	define BICOMC_EXPORT __declspec(dllexport)
#else
#	define BICOMC_EXPORT __attribute__((visibility("default")))
#endif

#define BICOMC_IS_NULLPTR_SUPPORT_COMPILER \
	(__has_feature(cxx_nullptr))
#define BICOMC_IS_STATIC_ASSERT_SUPPORT_COMPILER \
	(__has_feature(cxx_static_assert))

#if defined(_MSC_VER) && _MSC_VER >= 1900
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(1)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(1)
#else
#	define BICOMC_IS_THREAD_SAFE_STATIC_INIT_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif

#define BICOMC_IS_CONSTEXPR_SUPPORT_COMPILER \
	(__has_feature(cxx_constexpr))
#define BICOMC_IS_NOEXCEPT_SUPPORT_COMPILER \
	(__has_feature(cxx_noexcept))
#define BICOMC_IS_EXPLICITY_DEFAULT_DELETE_SUPPORT_COMPILER \
	(__has_feature(cxx_deleted_functions))
#define BICOMC_IS_MOVE_SEMANTIC_SUPPORT_COMPILER \
	(__has_feature(cxx_rvalue_references))
#define BICOMC_IS_VARIADIC_TEMPLATE_SUPPORT_COMPILER \
	(__has_feature(cxx_variadic_templates))

#if defined(_MSC_VER) && _MSC_VER >= 1900 
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(1)
#elif
#	define BICOMC_IS_CHAR_16_32_SUPPORT_COMPILER \
		(__cplusplus >= 201103L || defined(__GXX_EXPERIMENTAL_CXX0X__))
#endif

#define BICOMC_IS_STD_INT_SUPPORT_COMPILER \
	(__has_include(<cstdint>))
#define BICOMC_IS_TYPE_TRAITS_SUPPORT_COMPILER	\
	(__has_include(<type_traits>))
#define BICOMC_IS_MUTEX_SUPPORT_COMPILER \
	(__has_include(<mutex>))
#define BICOMC_IS_CODE_CVT_UTF8_SUPPORT_COMPILER \
	(__has_include(<codecvt>))
#define BICOMC_IS_EXTENED_EQUAL_SUPPORT_COMPILER \
	(__cplusplus >= 201103L)
#define BICOMC_IS_ARRAY_SUPPORT_COMPILER \
	(__has_include(<array>))

#endif // !def BICOMC_DETAIL_COMPILER_CLANG_H__
