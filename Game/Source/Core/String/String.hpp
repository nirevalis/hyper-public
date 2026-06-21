// Wykonane przez Piotra Chudzińskiego w dniu 10.04.2025

#pragma once

#include <cstring>
#include <string>
#include "../Macros.hpp"

/// <summary>
/// Represents a mutable sequence of characters, providing various member functions for string manipulation, comparison,
/// and conversion.
/// </summary>
class HYPER_API String
{
private:
	char* m_Data;
	int32 m_Length;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	String() : m_Data(nullptr), m_Length(0)
	{
	};

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	/// <param name="data"></param>
	String(const std::string& data) : m_Data(nullptr), m_Length(0)
	{
		Set(const_cast< char* >(data.data()), static_cast< int32 >(data.size()));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	/// <param name="data"></param>
	String(const char* data) : m_Data(nullptr), m_Length(0)
	{
		Set(const_cast< char* >(data), static_cast< int32 >(strlen(data)));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	String(const char* data, const int32 length) : m_Data(nullptr), m_Length(0)
	{
		Set(const_cast< char* >(data), m_Length);
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	String(const char* start, const char* end) : m_Data(nullptr), m_Length(0)
	{
		Set(const_cast< char* >(start), static_cast< int32 >(end - start));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	explicit String(char c) : m_Data(nullptr), m_Length(0)
	{
		Set(&c, 1);
	}

	~String()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="String"/> class.
	/// </summary>
	String(const String& str) : m_Data(nullptr), m_Length(0)
	{
		if (str.Get())
		{
			Set(str.Get(), str.m_Length);
		}
	};

	String(String&& str) noexcept : m_Data(str.m_Data), m_Length(str.m_Length)
	{
		str.m_Data = nullptr;
		str.m_Length = 0;
	};

	String& operator=(const String& str)
	{
		if (this == &str)
			return *this;
		Set(str.Get(), str.m_Length);

		return *this;
	}

	String& operator=(String&& str) noexcept
	{
		if (this == &str)
			return *this;
		Set(str.Get(), str.m_Length);

		free(str.m_Data);
		str.m_Length = 0;
		str.m_Data = nullptr;

		return *this;
	}

	bool operator==(const String& str) const
	{
		return strcmp(this->Get(), str.Get()) == 0;
	};

	String operator+(const char* str) const
	{
		int32 newLength = this->GetLength() + static_cast< int32 >(strlen(str)) + 1;
		char* data = new char[newLength];

		memcpy(data, this->Get(), this->GetLength() * sizeof(wchar_t));
		strcat(data, str);

		String concated(data, newLength);

		delete[] data;
		return concated;
	};

	String operator+(const String& str) const
	{
		int32 newLength = this->GetLength() + static_cast< int32 >(strlen(str.Get())) + 1;
		char* data = new char[newLength];

		memcpy(data, this->Get(), this->GetLength() * sizeof(wchar_t));
		strcat(data, str.Get());

		String concated(data, newLength);

		delete[] data;
		return concated;
	};

	/// <summary>
	/// Sets an array of characters to the string.
	/// </summary>
	/// <param name="data">The pointer to the start of an array of characters to set (UTF-8). This array need not be
	/// null-terminated, and null characters are not treated specially.</param> <param name="length">The number of
	/// characters to assign.</param>
	void Set(char* data, int32 length);

	/// <summary>
	/// Use only when string is empty.
	///
	/// </summary>
	/// <param name="length"></param>
	void Allocate(int32 length);

	/// <summary>
	/// Gets a raw char sequence
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] FORCE_INLINE char* Get() const
	{
		return m_Data;
	}

	[[nodiscard]] FORCE_INLINE int32 GetLength() const
	{
		return m_Length;
	}

	[[nodiscard]] bool StartsWith(const String& other) const;

	[[nodiscard]] bool EndsWith(const String& other) const;

	[[nodiscard]] int32 HashCode() const;
};
