#ifndef PGR_ENGINE_INCLUDE_UTILITY_PACKED
#define PGR_ENGINE_INCLUDE_UTILITY_PACKED

#ifdef __GNUC__
#define PACKED_BEGIN #pragma pack(push,1)
#define PACKED_END #pragma pack(pop)
#endif

#ifdef _MSC_VER
#define PACKED_BEGIN __pragma(pack(push,1))
#define PACKED_END __pragma(pack(pop))
#endif

#endif /* PGR_ENGINE_INCLUDE_UTILITY_PACKED */
