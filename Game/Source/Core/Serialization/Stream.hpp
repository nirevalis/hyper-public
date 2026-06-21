#pragma once

#include <Core/Common.hpp>
#include <ostream>
#include <istream>
#include <Core/GUID.hpp>
#include <Core/Math/Float3.hpp>
#include <Core/Math/Quaternion.hpp>

namespace Hyper
{
	/// <summary>
	/// Narzędzia pomagające czytanie i zapisywanie danych do strumienia.
	/// </summary>
	class HYPER_API Stream
	{
	public:
		/// <summary>
		/// Zapisuje liczbę całkowitą 32-bitową do strumienia.
		/// </summary>
		static void WriteInt32(std::ostream& stream, int32 value);

		/// <summary>
		/// Zapisuje liczbę całkowitą 32-bitową niepodpisaną do strumienia.
		/// </summary>
		static void WriteUInt32(std::ostream& stream, uint32 value);

		/// <summary>
		/// Zapisuje ciąg znaków do strumienia.
		/// </summary>
		static void WriteString(std::ostream& stream, const String& value);

		template <typename TEnum>
		static void WriteEnum(std::ostream& stream, TEnum value)
		{
			WriteUInt32(stream, static_cast< uint32 >(value));
		}

		template <typename TEnum>
		static TEnum ReadEnum(std::istream& stream)
		{
			return static_cast< TEnum >(ReadUInt32(stream));
		}

		static void WriteUInt64(std::ostream& stream, uint64 value);
		static uint64 ReadUInt64(std::istream& stream);
		static void WriteInt64(std::ostream& stream, int64 value);
		static int64 ReadInt64(std::istream& stream);
		static void WriteFloat(std::ostream& stream, float value);
		static void WriteQuaternion(std::ostream& stream, const Quaternion& quaternion);
		static void WriteFloat3(std::ostream& strea, const Float3& value);
		static void WriteGuid(std::ostream& stream, const Guid& guid);
		static float ReadFloat(std::istream& stream);
		static Float3 ReadFloat3(std::istream& stream);
		static Quaternion ReadQuaternion(std::istream& stream);
		static Guid ReadGuid(std::istream& stream);

		/// <summary>
		/// Czyta liczbę całkowitą 32-bitową ze strumienia.
		/// </summary>
		static int32 ReadInt32(std::istream& stream);

		/// <summary>
		/// Czyta liczbe całkowitą 32-bitową niepodpisana ze strumienia.
		/// </summary>
		static uint32 ReadUInt32(std::istream& stream);

		/// <summary>
		/// Czyta ciąg znaków do strumienia.
		/// </summary>
		static String ReadString(std::istream& stream);
	};
}
