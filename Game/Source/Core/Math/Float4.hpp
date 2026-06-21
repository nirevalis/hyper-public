#pragma once

#include <Core/Common.hpp>
#include "Float3.hpp"

namespace Hyper
{
	struct HYPER_API Float4
	{
		union
		{
			struct
			{
				float X, Y, Z, W;
			};

			float Raw[4];
		};
		Float4() : X(0), Y(0), Z(0), W(0)
		{

		}
		Float4(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w)
		{

		}

		Float4(Float3 XYZ, float w) : X(XYZ.X), Y(XYZ.Y), Z(XYZ.Z), W(w)
		{

		}

		const static Float4 Zero;
		const static Float4 One;

		float* GetValuePointer()
		{
			return Raw;
		}

		Float4 operator+(Float4 right) const
		{
			return Float4(X + right.X, Y + right.Y, Z + right.Z, W + right.W);
		}

		Float4 operator-(Float4 right) const
		{
			return Float4(X - right.X, Y - right.Y, Z - right.Z, W - right.W);
		}

		Float4 operator+=(Float4 right) const
		{
			Float4 clone = Float4(X, Y, Z, W);
			clone.X += right.X;
			clone.Y += right.Y;
			clone.Z += right.Z;
			clone.W += right.W;
			return clone;
		}

		Float4 operator-=(Float4 right) const
		{
			Float4 clone = Float4(X, Y, Z, W);
			clone.X -= right.X;
			clone.Y -= right.Y;
			clone.Z -= right.Z;
			clone.W -= right.W;
			return clone;
		}

		Float4 operator*(Float4 right) const
		{
			return Float4(X * right.X, Y * right.Y, Z * right.Z, W * right.W);
		}

		Float4 operator*(float value) const
		{
			return Float4(X * value, Y * value, Z * value, W * value);
		}

		Float4 operator/(float value) const
		{
			return Float4(X / value, Y / value, Z / value, W / value);
		}
	};
}