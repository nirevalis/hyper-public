//Wykonane przez Piotra Chudzińskiego w dniu 10.04.2025

#pragma once

#include "../Macros.hpp"
#include <cstring>
#include <string>

/// <summary>
/// Represents a mutable sequence of utf-16 characters, providing various member functions for string manipulation, comparison, and conversion.
/// </summary>
class HYPER_API WideString
{
private:
	wchar_t* m_Data;
	int32 m_Length;

public:
	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	WideString() : m_Data(nullptr),
		m_Length(0)
	{
	};

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	/// <param name="data"></param>
	WideString(const std::wstring& data) : m_Data(nullptr),
		m_Length(0)
	{
		Set(const_cast< wchar_t* >(data.data()), static_cast< int32 >(data.size()));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	/// <param name="data"></param>
	WideString(const wchar_t* data) : m_Data(nullptr),
		m_Length(0)
	{
		Set(const_cast< wchar_t* >(data), static_cast< int32 >(wcslen(data)));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	WideString(const wchar_t* data, int32 length) : m_Data(nullptr),
		m_Length(0)
	{
		Set(const_cast< wchar_t* >(data), length);
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	WideString(const wchar_t* start, const wchar_t* end) : m_Data(nullptr),
		m_Length(0)
	{
		Set(const_cast< wchar_t* >(start), static_cast< int32 >(end - start));
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	explicit WideString(wchar_t c) : m_Data(nullptr),
		m_Length(0)
	{
		Set(&c, 1);
	}

	~WideString()
	{
		if (m_Data)
		{
			free(m_Data);
			m_Data = nullptr;
		}
	}

	/// <summary>
	/// Initializes a new instance of the <see cref="WideString"/> class.
	/// </summary>
	WideString(const WideString& str) : m_Data(nullptr),
		m_Length(0)
	{
		if (str.Get())
		{
			Set(str.Get(), str.m_Length);
		}
	};

	WideString(WideString&& str)  noexcept : m_Data(str.m_Data),
		m_Length(str.m_Length)
	{
		str.m_Data = nullptr;
		str.m_Length = 0;
	};

	WideString& operator=(const WideString& str)
	{
		if (this == &str)
			return *this;
		Set(str.Get(), str.m_Length);

		return *this;
	}

	WideString& operator=(WideString&& str)
		noexcept
	{
		if (this == &str)
			return *this;
		Set(str.Get(), str.m_Length);

		free(str.m_Data);
		str.m_Length = 0;
		str.m_Data = nullptr;

		return *this;
	}

	bool operator==(const WideString& str) const
	{
		return wcscmp(this->Get(), str.Get()) == 0;
	};

	WideString operator+(const wchar_t* str) const
	{
		int32 newLength = this->GetLength() + static_cast< int32 >(wcslen(str)) + 1;
		wchar_t* data = new wchar_t[newLength];

		memcpy(data, this->Get(), this->GetLength() * sizeof(wchar_t));
		wcscat(data, str);

		WideString concated(data, newLength);

		delete[] data;
		return concated;
	};

	WideString operator+(const WideString& str) const
	{
		int32 newLength = this->GetLength() + static_cast< int32 >(wcslen(str.Get())) + 1;
		wchar_t* data = new wchar_t[newLength];

		memcpy(data, this->Get(), this->GetLength() * sizeof(wchar_t));
		wcscat(data, str.Get());

		WideString concated(data, newLength);

		delete[] data;
		return concated;
	};

	/// <summary>
	/// Sets an array of characters to the string.
	/// </summary>
	/// <param name="data">The pointer to the start of an array of characters to set (UTF-8). This array need not be null-terminated, and null characters are not treated specially.</param>
	/// <param name="length">The number of characters to assign.</param>
	void Set(wchar_t* data, int32 length);

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
	[[nodiscard]] inline wchar_t* Get() const
	{
		return m_Data;
	}

	[[nodiscard]] inline int32 GetLength() const
	{
		return m_Length;
	}

	[[nodiscard]] bool StartsWith(const WideString& other) const;

	[[nodiscard]] int32 HashCode() const;
};
