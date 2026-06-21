// Plik utworzony przez Piotra Chudzińskiego w dniu 10.04.2025

#include "StringUtil.hpp"
#include <codecvt>
#include <locale>

void String::Set(char* data, int32 length)
{
	if (m_Data)
	{
		delete m_Data;
		m_Data = nullptr;
	}

	Allocate(length);

	memcpy(m_Data, data, length * sizeof(char));
}

void String::Allocate(int32 length)
{
	m_Data = static_cast< char* >(malloc((length + 1) * sizeof(char)));
	m_Data[length] = ( char )'\0';

	m_Length = length;
}

bool String::StartsWith(const String& other) const
{
	const char* str = this->Get();
	const char* prefix = other.Get();

	while (*prefix)
	{
		if (*str != *prefix)
		{
			return false;
		}
		str++;
		prefix++;
	}
	return true;
}

bool String::EndsWith(const String& other) const
{
	const char* str = this->Get();
	const char* suffix = other.Get();

	size_t strLen = strlen(str);
	size_t suffixLen = strlen(suffix);

	if (suffixLen > strLen)
	{
		return false;
	}

	str += (strLen - suffixLen);

	while (*suffix)
	{
		if (*str != *suffix)
		{
			return false;
		}
		str++;
		suffix++;
	}
	return true;
}


int String::HashCode() const
{
	const auto* p = reinterpret_cast< const unsigned char* >(Get());
	uint32_t hash = 2166136261;

	for (size_t i = 0; i < GetLength(); ++i)
		hash = 16777619 * (hash ^ p[i]);

	return hash ^ (hash >> 16);
}

void WideString::Set(wchar_t* data, int32 length)
{
	if (m_Data)
	{
		delete m_Data;
		m_Data = nullptr;
	}

	Allocate(length);

	memcpy(m_Data, data, length * sizeof(wchar_t));
}

void WideString::Allocate(int32 length)
{
	m_Data = static_cast< wchar_t* >(malloc((length + 1) * sizeof(wchar_t)));
	m_Data[length] = ( char )'\0';

	m_Length = length;
}

bool WideString::StartsWith(const WideString& other) const
{
	const wchar_t* str = this->Get();
	const wchar_t* prefix = other.Get();

	while (*prefix)
	{
		if (*str != *prefix)
		{
			return false;
		}
		str++;
		prefix++;
	}
	return true;
}

int WideString::HashCode() const
{
	const auto* p = reinterpret_cast< const wchar_t* >(Get());
	uint32_t hash = 2166136261;

	for (size_t i = 0; i < GetLength(); ++i)
		hash = 16777619 * (hash ^ p[i]);

	return hash ^ (hash >> 16);
}

WideString StringUtil::GetUTF16String(const String& other)
{
	static std::wstring_convert<std::codecvt_utf8_utf16<wchar_t> > converter;
	return converter.from_bytes(other.Get());
}

String StringUtil::GetUTF8String(const WideString& other)
{
	static std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
	return converter.to_bytes(other.Get());
}
