#include "Stream.hpp"

namespace Hyper
{
	void Stream::WriteInt32(std::ostream& stream, int32 value)
	{
		stream.write(reinterpret_cast< const char* >(&value), sizeof(int32));
	}

	void Stream::WriteUInt32(std::ostream& stream, uint32 value)
	{
		stream.write(reinterpret_cast< const char* >(&value), sizeof(uint32));
	}

	void Stream::WriteString(std::ostream& stream, const String& value)
	{
		WriteUInt32(stream, value.GetLength());
		stream.write(value.Get(), value.GetLength());
	}

	void Stream::WriteUInt64(std::ostream& stream, uint64 value)
	{
		stream.write(reinterpret_cast< const char* >(&value), sizeof(uint64));
	}

	uint64 Stream::ReadUInt64(std::istream& stream)
	{
		uint64 value;
		stream.read(reinterpret_cast< char* >(&value), sizeof(uint64));

		return value;
	}

	void Stream::WriteInt64(std::ostream& stream, int64 value)
	{
		stream.write(reinterpret_cast< const char* >(&value), sizeof(int64));
	}

	int64 Stream::ReadInt64(std::istream& stream)
	{
		uint64 value;
		stream.read(reinterpret_cast< char* >(&value), sizeof(int64));

		return value;
	}

	void Stream::WriteFloat(std::ostream& stream, float value)
	{
		stream.write(reinterpret_cast< const char* >(&value), sizeof(float));
	}

	void Stream::WriteFloat3(std::ostream& strea, const Float3& value)
	{
		WriteFloat(strea, value.X);
		WriteFloat(strea, value.Y);
		WriteFloat(strea, value.Z);
	}

	void Stream::WriteQuaternion(std::ostream& strea, const Quaternion& value)
	{
		WriteFloat(strea, value.X);
		WriteFloat(strea, value.Y);
		WriteFloat(strea, value.Z);
		WriteFloat(strea, value.W);
	}

	void Stream::WriteGuid(std::ostream& stream, const Guid& guid)
	{
		WriteInt32(stream, guid.A);
		WriteInt32(stream, guid.B);
		WriteInt32(stream, guid.C);
		WriteInt32(stream, guid.D);
	}

	float Stream::ReadFloat(std::istream& stream)
	{
		float value;
		stream.read(reinterpret_cast< char* >(&value), sizeof(float));

		return value;
	}

	Float3 Stream::ReadFloat3(std::istream& stream)
	{
		Float3 vec;
		vec.X = ReadFloat(stream);
		vec.Y = ReadFloat(stream);
		vec.Z = ReadFloat(stream);

		return vec;
	}

	Quaternion Stream::ReadQuaternion(std::istream& stream)
	{
		Quaternion vec;
		vec.X = ReadFloat(stream);
		vec.Y = ReadFloat(stream);
		vec.Z = ReadFloat(stream);
		vec.W = ReadFloat(stream);

		return vec;
	}

	Guid Stream::ReadGuid(std::istream& stream)
	{
		Guid g;
		g.A = ReadInt32(stream);
		g.B = ReadInt32(stream);
		g.C = ReadInt32(stream);
		g.D = ReadInt32(stream);

		return g;
	}

	int32 Stream::ReadInt32(std::istream& stream)
	{
		int32 value;
		stream.read(reinterpret_cast< char* >(&value), sizeof(int32));

		return value;
	}

	uint32 Stream::ReadUInt32(std::istream& stream)
	{
		uint32 value;
		stream.read(reinterpret_cast< char* >(&value), sizeof(uint32));

		return value;
	}

	String Stream::ReadString(std::istream& stream)
	{
		uint32 length = ReadUInt32(stream);
		String value;
		value.Allocate(length);

		stream.read(value.Get(), length);
		return value;
	}
}