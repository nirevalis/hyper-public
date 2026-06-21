// Plik utworzony przez Piotra Chudzińskiego w dniu 11.04.2025
#pragma once

#include "String.hpp"
#include "WideString.hpp"
#include <format>

constexpr uint32_t XORStr(std::string_view str)
{
	uint32_t hash = 2166136261u;
	for (char c : str)
	{
		hash ^= static_cast< unsigned char >(c);
		hash *= 16777619u;
	}
	return hash;
}

/**
 * @brief A utility which contains essential methods for string operations.
 *
 */
class StringUtil
{
public:
	/**
	 * @brief Returns UTF-16 string converted from UTF-8 string.
	 *
	 */
	static WideString GetUTF16String(const String& other);

	/**
	 * @brief Returns UTF-8 string converted from UTF-16 string.
	 *
	 */
	static String GetUTF8String(const WideString& other);
};

template <>
struct std::formatter<WideString>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const WideString& s, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", StringUtil::GetUTF8String(s).Get());
	}
};

template <>
struct std::formatter<String>
{
	constexpr auto parse(std::format_parse_context& ctx)
	{
		return ctx.begin();
	}

	auto format(const String& s, std::format_context& ctx) const
	{
		return std::format_to(ctx.out(), "{}", s.Get());
	}
};

/**
 * @brief Implementation for hashing UTF-8 strings. Mainly used by maps.
 *
 */
template<>
struct std::hash<String>
{
	std::size_t operator()(const String& k) const noexcept
	{
		return k.HashCode();
	}
};

/**
 * @brief Implementation for hashing UTF-16 strings. Mainly used by maps.
 *
 */
template<>
struct std::hash<WideString>
{
	std::size_t operator()(const WideString& k) const noexcept
	{
		return k.HashCode();
	}
};

#define FORMAT(...) std::format(__VA_ARGS__)
