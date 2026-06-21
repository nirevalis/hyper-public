#pragma once

using int8 = char;
using int16 = short;
using int32 = int;
using int64 = long long;

using uint8 = unsigned char;
using uint16 = unsigned short;
using uint32 = unsigned int;
using uint64 = unsigned long long;

#if defined(_WIN32)
#if defined(HYPER_PLAYER_BUILD_DLL)
#define HYPER_API __declspec(dllexport)
#elif defined(HYPER_PLAYER_USE_DLL)
#define HYPER_API __declspec(dllimport)
#else
#define HYPER_API
#endif
#else
#define HYPER_API
#endif

#if defined(_MSC_VER)
#define FORCE_INLINE __forceinline
#elif defined(__GNUC__) || defined(__clang__)
#define FORCE_INLINE inline __attribute__((always_inline))
#else
#define FORCE_INLINE inline
#endif