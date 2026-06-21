#pragma once

#include <Core/Common.hpp>
#include "Math.hpp"
#include "Quaternion.hpp"

namespace Hyper
{
	/**
	 * @brief Row major ofc.
	 */
	struct HYPER_API Matrix3
	{
	public:
		union
		{
			float Values[3][3];
			float Raw[9];
		};
		
	
		// Transpose of the matrix
		Matrix3 Transpose() const
		{
			Matrix3 result;
			for (int r = 0; r < 3; ++r)
				for (int c = 0; c < 3; ++c)
					result.Values[r][c] = Values[c][r];
			return result;
		}

		// Inverse of the matrix
		Matrix3 Inverse() const
		{
			Matrix3 result;
			float det =
				  Values[0][0] * (Values[1][1]*Values[2][2] - Values[1][2]*Values[2][1])
				- Values[0][1] * (Values[1][0]*Values[2][2] - Values[1][2]*Values[2][0])
				+ Values[0][2] * (Values[1][0]*Values[2][1] - Values[1][1]*Values[2][0]);

			if (fabs(det) < 1e-8f)
			{
				// Degenerate, return identity or assert
				return Identity();
			}

			float invDet = 1.0f / det;

			result.Values[0][0] =  (Values[1][1]*Values[2][2] - Values[1][2]*Values[2][1]) * invDet;
			result.Values[0][1] = -(Values[0][1]*Values[2][2] - Values[0][2]*Values[2][1]) * invDet;
			result.Values[0][2] =  (Values[0][1]*Values[1][2] - Values[0][2]*Values[1][1]) * invDet;

			result.Values[1][0] = -(Values[1][0]*Values[2][2] - Values[1][2]*Values[2][0]) * invDet;
			result.Values[1][1] =  (Values[0][0]*Values[2][2] - Values[0][2]*Values[2][0]) * invDet;
			result.Values[1][2] = -(Values[0][0]*Values[1][2] - Values[0][2]*Values[1][0]) * invDet;

			result.Values[2][0] =  (Values[1][0]*Values[2][1] - Values[1][1]*Values[2][0]) * invDet;
			result.Values[2][1] = -(Values[0][0]*Values[2][1] - Values[0][1]*Values[2][0]) * invDet;
			result.Values[2][2] =  (Values[0][0]*Values[1][1] - Values[0][1]*Values[1][0]) * invDet;

			return result;
		}

		// Identity matrix (optional utility)
		static Matrix3 Identity()
		{
			Matrix3 I = {};
			I.Values[0][0] = I.Values[1][1] = I.Values[2][2] = 1.0f;
			return I;
		}
	};
}