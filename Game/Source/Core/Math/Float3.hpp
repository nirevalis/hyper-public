#pragma once

#include <Core/Common.hpp>
#include "Float2.hpp"
#include "Math.hpp"

namespace Hyper
{
	struct HYPER_API Float3
	{
		union
		{
			struct
			{
				float X, Y, Z;
			};

			float Raw[3];
		};
		Float3() : X(0), Y(0), Z(0)
		{

		}
		Float3(float x, float y, float z) : X(x), Y(y), Z(z)
		{

		}

		Float3(const Float2& xy, float z) : X(xy.X), Y(xy.Y), Z(z)
		{

		}

		const static Float3 Zero;
		const static Float3 One;

		float* GetValuePointer()
		{
			return Raw;
		}

		float GetMagnitude()
		{
			return Math::Sqrt(X * X + Y * Y + Z * Z);
		}

		bool operator==(const Float3& vec) const
		{
			return vec.X == X && vec.Y == Y && vec.Z == Z;
		}

		static Float3 Cross(const Float3& source, const Float3& other)
		{
			return Float3(
				source.Y * other.Z - source.Z * other.Y,
				source.Z * other.X - source.X * other.Z,
				source.X * other.Y - source.Y * other.X
			);
		}

		static float Dot(const Float3& left, const Float3& right)
		{
			return left.X * right.X + left.Y * right.Y + left.Z * right.Z;
		}

		void Normalize()
		{
			float length = Math::Sqrt(X * X + Y * Y + Z * Z);
			if (length >= Math::Epsilon)
			{
				float inv = 1.0f / length;
				X *= inv;
				Y *= inv;
				Z *= inv;
			}
		}

		Float3 operator+(Float3 right) const
		{
			return Float3(X + right.X, Y + right.Y, Z + right.Z);
		}

		Float3 operator-(Float3 right) const
		{
			return Float3(X - right.X, Y - right.Y, Z - right.Z);
		}

		Float3 operator-() const
		{
			return Float3(-X, -Y, -Z);
		}

		Float3 operator+=(Float3 right)
		{
			this->X += right.X;
			this->Y += right.Y;
			this->Z += right.Z;
			return *this;
		}

		Float3 operator-=(Float3 right)
		{
			this->X -= right.X;
			this->Y -= right.Y;
			this->Z -= right.Z;
			return *this;
		}

		Float3 operator*(Float3 right) const
		{
			return Float3(X * right.X, Y * right.Y, Z * right.Z);
		}

		Float3 operator*(float value) const
		{
			return Float3(X * value, Y * value, Z * value);
		}

		Float3 operator/(float value) const
		{
			return Float3(X / value, Y / value, Z / value);
		}
	};
}