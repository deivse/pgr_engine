#ifndef PGR_ENGINE_INCLUDE_UTILITY_ALIGNED
#define PGR_ENGINE_INCLUDE_UTILITY_ALIGNED

#if defined(__GNUC__) || defined(__clang__)
#define ALIGN(x) __attribute__((aligned(x)))
#elif defined(_MSC_VER)
#define ALIGN(x) __declspec(align(x))
#else
#error "Unknown compiler; can't define ALIGN"
#endif

#if defined(__GNUC__) || defined(__clang__)
#define ALIGNOF(X) __alignof__(X)
#elif defined(_MSC_VER)
#define ALIGNOF(X) __alignof(X)
#else
#error "Unknown compiler; can't define ALIGNOF"
#endif

#endif /* PGR_ENGINE_INCLUDE_UTILITY_ALIGNED */
