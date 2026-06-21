#pragma once

#include <Core/Common.hpp>
#include "Math.hpp"
#include "Quaternion.hpp"

namespace Hyper
{
	/**
	 * @brief Row major ofc.
	 */
	struct HYPER_API Matrix4
	{
	public:
		union
		{
			struct
			{
				float M11;
				float M12;
				float M13;
				float M14;
				float M21;
				float M22;
				float M23;
				float M24;
				float M31;
				float M32;
				float M33;
				float M34;
				float M41;
				float M42;
				float M43;
				float M44;
			};

			float Values[4][4];
			float Raw[16];
		};

		static const Matrix4 Zero;
		static const Matrix4 Identity;

		Matrix4()
			: M11(0)
			, M12(0)
			, M13(0)
			, M14(0)
			, M21(0)
			, M22(0)
			, M23(0)
			, M24(0)
			, M31(0)
			, M32(0)
			, M33(0)
			, M34(0)
			, M41(0)
			, M42(0)
			, M43(0)
			, M44(0)
		{
		}

		Matrix4(float m11, float m12, float m13, float m14,
			float m21, float m22, float m23, float m24,
			float m31, float m32, float m33, float m34,
			float m41, float m42, float m43, float m44)
			: M11(m11)
			, M12(m12)
			, M13(m13)
			, M14(m14)
			, M21(m21)
			, M22(m22)
			, M23(m23)
			, M24(m24)
			, M31(m31)
			, M32(m32)
			, M33(m33)
			, M34(m34)
			, M41(m41)
			, M42(m42)
			, M43(m43)
			, M44(m44)
		{
		}

		float* GetValuePointer()
		{
			return Raw;
		}

		static Float4 Multiply(const Matrix4& mat, const Float4& vec)
		{
			return Float4{
				mat.Values[0][0]*vec.X + mat.Values[0][1]*vec.Y + mat.Values[0][2]*vec.Z + mat.Values[0][3]*vec.W,
				mat.Values[1][0]*vec.X + mat.Values[1][1]*vec.Y + mat.Values[1][2]*vec.Z + mat.Values[1][3]*vec.W,
				mat.Values[2][0]*vec.X + mat.Values[2][1]*vec.Y + mat.Values[2][2]*vec.Z + mat.Values[2][3]*vec.W,
				mat.Values[3][0]*vec.X + mat.Values[3][1]*vec.Y + mat.Values[3][2]*vec.Z + mat.Values[3][3]*vec.W
			};
		}

		static Matrix4 AffineInverse(Matrix4 value)
		{
			Matrix4 result;
			const float d11 = value.M22 * value.M33 + value.M23 * -value.M32;
			const float d12 = value.M21 * value.M33 + value.M23 * -value.M31;
			const float d13 = value.M21 * value.M32 + value.M22 * -value.M31;

			float det = value.M11 * d11 - value.M12 * d12 + value.M13 * d13;

			det = 1.0f / det;

			const float d21 = value.M12 * value.M33 + value.M13 * -value.M32;
			const float d22 = value.M11 * value.M33 + value.M13 * -value.M31;
			const float d23 = value.M11 * value.M32 + value.M12 * -value.M31;

			const float d31 = value.M12 * value.M23 - value.M13 * value.M22;
			const float d32 = value.M11 * value.M23 - value.M13 * value.M21;
			const float d33 = value.M11 * value.M22 - value.M12 * value.M21;

			Matrix4 inverted = Matrix4(
				+d11 * det,
				-d21 * det,
				+d31 * det,
				0,

				-d12 * det,
				+d22 * det,
				-d32 * det,
				0,

				+d13 * det,
				-d23 * det,
				+d33 * det,
				0,

				0, 0, 0, 0
			);



			return result;
		}

		static Matrix4 Invert(Matrix4 value)
		{
			Matrix4 result;
			const float b0 = value.M31 * value.M42 - value.M32 * value.M41;
			const float b1 = value.M31 * value.M43 - value.M33 * value.M41;
			const float b2 = value.M34 * value.M41 - value.M31 * value.M44;
			const float b3 = value.M32 * value.M43 - value.M33 * value.M42;
			const float b4 = value.M34 * value.M42 - value.M32 * value.M44;
			const float b5 = value.M33 * value.M44 - value.M34 * value.M43;

			const float d11 = value.M22 * b5 + value.M23 * b4 + value.M24 * b3;
			const float d12 = value.M21 * b5 + value.M23 * b2 + value.M24 * b1;
			const float d13 = value.M21 * -b4 + value.M22 * b2 + value.M24 * b0;
			const float d14 = value.M21 * b3 + value.M22 * -b1 + value.M23 * b0;

			float det = value.M11 * d11 - value.M12 * d12 + value.M13 * d13 - value.M14 * d14;
			if (Math::Abs(det) <= 1e-12f)
			{
				result = Zero;
				return result;
			}

			det = 1.0f / det;

			const float a0 = value.M11 * value.M22 - value.M12 * value.M21;
			const float a1 = value.M11 * value.M23 - value.M13 * value.M21;
			const float a2 = value.M14 * value.M21 - value.M11 * value.M24;
			const float a3 = value.M12 * value.M23 - value.M13 * value.M22;
			const float a4 = value.M14 * value.M22 - value.M12 * value.M24;
			const float a5 = value.M13 * value.M24 - value.M14 * value.M23;

			const float d21 = value.M12 * b5 + value.M13 * b4 + value.M14 * b3;
			const float d22 = value.M11 * b5 + value.M13 * b2 + value.M14 * b1;
			const float d23 = value.M11 * -b4 + value.M12 * b2 + value.M14 * b0;
			const float d24 = value.M11 * b3 + value.M12 * -b1 + value.M13 * b0;

			const float d31 = value.M42 * a5 + value.M43 * a4 + value.M44 * a3;
			const float d32 = value.M41 * a5 + value.M43 * a2 + value.M44 * a1;
			const float d33 = value.M41 * -a4 + value.M42 * a2 + value.M44 * a0;
			const float d34 = value.M41 * a3 + value.M42 * -a1 + value.M43 * a0;

			const float d41 = value.M32 * a5 + value.M33 * a4 + value.M34 * a3;
			const float d42 = value.M31 * a5 + value.M33 * a2 + value.M34 * a1;
			const float d43 = value.M31 * -a4 + value.M32 * a2 + value.M34 * a0;
			const float d44 = value.M31 * a3 + value.M32 * -a1 + value.M33 * a0;

			result.M11 = +d11 * det;
			result.M12 = -d21 * det;
			result.M13 = +d31 * det;
			result.M14 = -d41 * det;
			result.M21 = -d12 * det;
			result.M22 = +d22 * det;
			result.M23 = -d32 * det;
			result.M24 = +d42 * det;
			result.M31 = +d13 * det;
			result.M32 = -d23 * det;
			result.M33 = +d33 * det;
			result.M34 = -d43 * det;
			result.M41 = -d14 * det;
			result.M42 = +d24 * det;
			result.M43 = -d34 * det;
			result.M44 = +d44 * det;

			return result;
		}

		static Matrix4 LookAt(Float3 eye, Float3 target, Float3 up)
		{
			Matrix4 result = Identity;

			Float3 f = target - eye;
			f.Normalize();

			Float3 s = Float3::Cross(up, f);
			s.Normalize();

			Float3 u = Float3::Cross(f, s);

			result.Values[0][0] = s.X;
			result.Values[1][0] = s.Y;
			result.Values[2][0] = s.Z;

			result.Values[0][1] = u.X;
			result.Values[1][1] = u.Y;
			result.Values[2][1] = u.Z;

			result.Values[0][2] = f.X;
			result.Values[1][2] = f.Y;
			result.Values[2][2] = f.Z;

			result.Values[3][0] = -Float3::Dot(s, eye);
			result.Values[3][1] = -Float3::Dot(u, eye);
			result.Values[3][2] = -Float3::Dot(f, eye);

			return result;
		}

		static Matrix4 FromQuaternion(Quaternion quat)
		{
			Matrix4 result;

			result.Values[0][0] = 1 - 2 * (quat.Y * quat.Y + quat.Z * quat.Z);
			result.Values[0][1] = 2 * (quat.X * quat.Y - quat.W * quat.Z);
			result.Values[0][2] = 2 * (quat.X * quat.Z + quat.W * quat.Y);
			result.Values[0][3] = 0;

			result.Values[1][0] = 2 * (quat.X * quat.Y + quat.W * quat.Z);
			result.Values[1][1] = 1 - 2 * (quat.X * quat.X + quat.Z * quat.Z);
			result.Values[1][2] = 2 * (quat.Y * quat.Z - quat.W * quat.X);
			result.Values[1][3] = 0;

			result.Values[2][0] = 2 * (quat.X * quat.Z - quat.W * quat.Y);
			result.Values[2][1] = 2 * (quat.Y * quat.Z + quat.W * quat.X);
			result.Values[2][2] = 1 - 2 * (quat.X * quat.X + quat.Y * quat.Y);
			result.Values[2][3] = 0;

			result.Values[3][0] = 0;
			result.Values[3][1] = 0;
			result.Values[3][2] = 0;
			result.Values[3][3] = 1;

			return result;
		}

		static Matrix4 Transpose(Matrix4 value)
		{
			Matrix4 result = Matrix4::Zero;
			result.M11 = value.M11;
			result.M12 = value.M21;
			result.M13 = value.M31;
			result.M14 = value.M41;
			result.M21 = value.M12;
			result.M22 = value.M22;
			result.M23 = value.M32;
			result.M24 = value.M42;
			result.M31 = value.M13;
			result.M32 = value.M23;
			result.M33 = value.M33;
			result.M34 = value.M43;
			result.M41 = value.M14;
			result.M42 = value.M24;
			result.M43 = value.M34;
			result.M44 = value.M44;
			return result;
		}

		static Matrix4 Translate(Float3 Value)
		{
			Matrix4 Source = Identity;
			Source.M41 = Value.X;
			Source.M42 = Value.Y;
			Source.M43 = Value.Z;
			return Source;
		}

		static Matrix4 Scale(Float3 Value)
		{
			Matrix4 Source = Identity;
			Source.M11 *= Value.X;
			Source.M12 *= Value.X;
			Source.M13 *= Value.X;
			Source.M21 *= Value.Y;
			Source.M22 *= Value.Y;
			Source.M23 *= Value.Y;
			Source.M31 *= Value.Z;
			Source.M32 *= Value.Z;
			Source.M33 *= Value.Z;

			return Source;
		}

		static Matrix4 Perspective(float fov, float aspectRatio, float zNear, float zFar)
		{
			Matrix4 result = Matrix4::Zero;

			float tanHalf = Math::Tan(fov * 0.5f);

			result.Values[0][0] = 1.0f / (aspectRatio * tanHalf);
			result.Values[1][1] = 1.0f / tanHalf;
			result.Values[2][2] = (zFar + zNear) / (zFar - zNear);
			result.Values[2][3] = 1;
			result.Values[3][2] = -(2 * zFar * zNear) / (zFar - zNear);

			return result;
		}

		static Matrix4 Orthographic(float left, float right, float bottom, float top, float zNear, float zFar)
		{
			Matrix4 result = Identity;
			result.Values[0][0] = 2 / (right - left);
			result.Values[1][1] = 2 / (top - bottom);
			result.Values[2][2] = -1 / (zFar - zNear);
			result.Values[3][0] = -(right + left) / (right - left);
			result.Values[3][1] = -(top + bottom) / (top - bottom);
			result.Values[3][2] = -(zNear) / (zFar - zNear);

			return result;
		}

		static Matrix4 OrthographicShadows(float left, float right, float bottom, float top, float zNear, float zFar)
		{

			Matrix4 result = Identity;
			result.Values[0][0] = 2 / (right - left);
			result.Values[1][1] = 2 / (top - bottom);
			result.Values[2][2] = 1 / (zFar - zNear);
			result.Values[3][0] = -(right + left) / (right - left);
			result.Values[3][1] = -(top + bottom) / (top - bottom);
			result.Values[3][2] = zNear / (zNear - zFar);

			return result;
		}


		Matrix4 operator-()
		{
			return Invert(*this);
		}

		Matrix4 operator*(float right)
		{
			Matrix4 result;
			result.M11 = M11 * right;
			result.M12 = M12 * right;
			result.M13 = M13 * right;
			result.M14 = M14 * right;
			result.M21 = M21 * right;
			result.M22 = M22 * right;
			result.M23 = M23 * right;
			result.M24 = M24 * right;
			result.M31 = M31 * right;
			result.M32 = M32 * right;
			result.M33 = M33 * right;
			result.M34 = M34 * right;
			result.M41 = M41 * right;
			result.M42 = M42 * right;
			result.M43 = M43 * right;
			result.M44 = M44 * right;

			return result;
		}

		Matrix4 operator*(Matrix4 right)
		{
			Matrix4 result = Matrix4::Zero;
			Matrix4 left = *this;
			result.M11 = left.M11 * right.M11 + left.M12 * right.M21 + left.M13 * right.M31 + left.M14 * right.M41;
			result.M12 = left.M11 * right.M12 + left.M12 * right.M22 + left.M13 * right.M32 + left.M14 * right.M42;
			result.M13 = left.M11 * right.M13 + left.M12 * right.M23 + left.M13 * right.M33 + left.M14 * right.M43;
			result.M14 = left.M11 * right.M14 + left.M12 * right.M24 + left.M13 * right.M34 + left.M14 * right.M44;
			result.M21 = left.M21 * right.M11 + left.M22 * right.M21 + left.M23 * right.M31 + left.M24 * right.M41;
			result.M22 = left.M21 * right.M12 + left.M22 * right.M22 + left.M23 * right.M32 + left.M24 * right.M42;
			result.M23 = left.M21 * right.M13 + left.M22 * right.M23 + left.M23 * right.M33 + left.M24 * right.M43;
			result.M24 = left.M21 * right.M14 + left.M22 * right.M24 + left.M23 * right.M34 + left.M24 * right.M44;
			result.M31 = left.M31 * right.M11 + left.M32 * right.M21 + left.M33 * right.M31 + left.M34 * right.M41;
			result.M32 = left.M31 * right.M12 + left.M32 * right.M22 + left.M33 * right.M32 + left.M34 * right.M42;
			result.M33 = left.M31 * right.M13 + left.M32 * right.M23 + left.M33 * right.M33 + left.M34 * right.M43;
			result.M34 = left.M31 * right.M14 + left.M32 * right.M24 + left.M33 * right.M34 + left.M34 * right.M44;
			result.M41 = left.M41 * right.M11 + left.M42 * right.M21 + left.M43 * right.M31 + left.M44 * right.M41;
			result.M42 = left.M41 * right.M12 + left.M42 * right.M22 + left.M43 * right.M32 + left.M44 * right.M42;
			result.M43 = left.M41 * right.M13 + left.M42 * right.M23 + left.M43 * right.M33 + left.M44 * right.M43;
			result.M44 = left.M41 * right.M14 + left.M42 * right.M24 + left.M43 * right.M34 + left.M44 * right.M44;

			return result;
		}

		Matrix4 ToColumnMajor() const
		{
			Matrix4 result;
			for (int row = 0; row < 4; ++row)
			{
				for (int col = 0; col < 4; ++col)
				{
					result.Values[col][row] = this->Values[row][col];
				}
			}
			return result;
		}
	};
}
