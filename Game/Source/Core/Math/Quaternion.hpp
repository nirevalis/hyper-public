#pragma once

#include <Core/Common.hpp>
#include "Math.hpp"
#include "Quaternion.hpp"
#include "Float4.hpp"
#include <cmath>

namespace Hyper
{
	struct HYPER_API Quaternion
	{
		union
		{
			struct
			{
				float X, Y, Z, W;
			};

			float Raw[4];
		};
		Quaternion() : X(0), Y(0), Z(0), W(0)
		{

		}
		Quaternion(float x, float y, float z, float w) : X(x), Y(y), Z(z), W(w)
		{

		}

		explicit Quaternion(Float4 value) : X(value.X), Y(value.Y), Z(value.Z), W(value.W)
		{

		}

		Quaternion Conjugate() const
		{
			return { -X, -Y, -Z, W };
		}

		// Fixed Euler angle conversion to match Jolt Physics
		// Rotation order: X then Y then Z (RotZ * RotY * RotX)
		static Quaternion Euler(Float3 angles)
		{
			// Convert degrees to radians and divide by 2
			const float halfX = Math::HalfRadians(angles.X);  // Roll around X
			const float halfY = Math::HalfRadians(angles.Y);  // Pitch around Y
			const float halfZ = Math::HalfRadians(angles.Z);  // Yaw around Z

			const float cx = Math::Cos(halfX);
			const float sx = Math::Sin(halfX);
			const float cy = Math::Cos(halfY);
			const float sy = Math::Sin(halfY);
			const float cz = Math::Cos(halfZ);
			const float sz = Math::Sin(halfZ);

			// This matches Jolt's sEulerAngles implementation
			return Quaternion(
				cz * sx * cy - sz * cx * sy,  // X
				cz * cx * sy + sz * sx * cy,  // Y
				sz * cx * cy - cz * sx * sy,  // Z
				cz * cx * cy + sz * sx * sy   // W
			);
		}

		static Quaternion Normalize(const Quaternion& q)
		{
			float length = Math::Sqrt(q.X * q.X + q.Y * q.Y + q.Z * q.Z + q.W * q.W);
			return q / length;
		}


		// Fixed ToEuler to match Jolt's GetEulerAngles
		static Float3 ToEuler(const Quaternion& q)
		{
			float y_sq = q.Y * q.Y;

			// X (Roll)
			float t0 = 2.0f * (q.W * q.X + q.Y * q.Z);
			float t1 = 1.0f - 2.0f * (q.X * q.X + y_sq);

			// Y (Pitch)
			float t2 = 2.0f * (q.W * q.Y - q.Z * q.X);
			t2 = Math::Clamp(t2, -1.0f, 1.0f);  // Clamp to avoid NaN from asin

			// Z (Yaw)
			float t3 = 2.0f * (q.W * q.Z + q.X * q.Y);
			float t4 = 1.0f - 2.0f * (y_sq + q.Z * q.Z);

			return Float3(
				Math::Atan2(t0, t1),  // X (Roll)
				Math::Asin(t2),       // Y (Pitch)
				Math::Atan2(t3, t4)   // Z (Yaw)
			);
		}


		void Multiply(const Quaternion& other)
		{
			const float a = Y * other.Z - Z * other.Y;
			const float b = Z * other.X - X * other.Z;
			const float c = X * other.Y - Y * other.X;
			const float d = X * other.X + Y * other.Y + Z * other.Z;
			X = X * other.W + other.X * W + a;
			Y = Y * other.W + other.Y * W + b;
			Z = Z * other.W + other.Z * W + c;
			W = W * other.W - d;
		}

		float* GetValuePointer()
		{
			return Raw;
		}

		Quaternion operator+(const Quaternion& right) const
		{
			return Quaternion(X + right.X, Y + right.Y, Z + right.Z, W + right.W);
		}

		Quaternion operator-(const Quaternion& right) const
		{
			return Quaternion(X - right.X, Y - right.Y, Z - right.Z, W - right.W);
		}

		Float3 operator*(const Float3& right) const
		{
			Quaternion vQuat { right.X, right.Y, right.Z, 0.0f };
			Quaternion result = (*this) * vQuat * this->Conjugate();
			return { result.X, result.Y, result.Z };
		}

		Quaternion operator*(const Quaternion& q) const
		{
			return {
			   X * q.W + W * q.X + Y * q.Z - Z * q.Y,
			   Y * q.W + W * q.Y + Z * q.X - X * q.Z,
			   Z * q.W + W * q.Z + X * q.Y - Y * q.X,
			   W * q.W - X * q.X - Y * q.Y - Z * q.Z
			};
		}

		Quaternion operator*(float value) const
		{
			return Quaternion(X * value, Y * value, Z * value, W * value);
		}

		Quaternion operator/(float value) const
		{
			return Quaternion(X / value, Y / value, Z / value, W / value);
		}
	};
}