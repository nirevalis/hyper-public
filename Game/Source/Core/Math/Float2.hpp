#pragma once

#include <Core/Common.hpp>

namespace Hyper
{
	struct HYPER_API Float2
	{
		union
		{
			struct
			{
				float X, Y;
			};

			float Raw[2];
		};

		Float2() : X(0), Y(0)
		{

		}
		Float2(float x, float y) : X(x), Y(y)
		{

		}

		const static Float2 Zero;
		const static Float2 One;

		float* GetValuePointer()
		{
			return Raw;
		}

		bool operator==(Float2 right) const
		{
			return X == right.X && Y == right.Y;
		}

		Float2 operator+(Float2 right) const
		{
			return Float2(X + right.X, Y + right.Y);
		}

		Float2 operator-(Float2 right) const
		{
			return Float2(X - right.X, Y - right.Y);
		}

		bool operator<(Float2 right)
		{
			return X < right.X && Y < right.Y;
		}

		bool operator>(Float2 right)
		{
			return X > right.X && Y > right.Y;
		}

		bool operator<=(Float2 right)
		{
			return X < right.X && Y < right.Y;
		}

		bool operator>=(Float2 right)
		{
			return X >= right.X && Y >= right.Y;
		}

		Float2 operator+=(Float2 right) const
		{
			Float2 clone = Float2(X, Y);
			clone.X += right.X;
			clone.Y += right.Y;
			return clone;
		}

		Float2 operator-=(Float2 right) const
		{
			Float2 clone = Float2(X, Y);
			clone.X -= right.X;
			clone.Y -= right.Y;
			return clone;
		}

		Float2 operator*(Float2 right) const
		{
			return Float2(X * right.X, Y * right.Y);
		}

		Float2 operator*(float value) const
		{
			return Float2(X * value, Y * value);
		}

		Float2 operator/(float value) const
		{
			return Float2(X / value, Y / value);
		}
	};
}