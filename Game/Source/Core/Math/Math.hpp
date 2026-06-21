#pragma once

#include <Core/Common.hpp>
#include <cmath>

namespace Hyper
{
	class HYPER_API Math
	{
	public:
		constexpr static float PI = 3.14159265359f;
		constexpr static float Epsilon = 1e-6f;

		template<typename T>
		constexpr inline static T Radians(T value)
		{
			return value * (PI / 180.0f);
		}

		template<typename T>
		constexpr inline static T HalfRadians(T value)
		{
			return value * ((PI / 180.0f) * 0.5f);
		}

		template<typename T>
		constexpr inline static T Tan(T value)
		{
			return tan(value);
		}

		template<typename T>
		constexpr inline static T Sin(T value)
		{
			return sin(value);
		}

		template<typename T>
		constexpr inline static T Cos(T value)
		{
			return cos(value);
		}

		template<typename T>
		constexpr inline static T Abs(T value)
		{
			return abs(value);
		}

		template<typename T>
		constexpr inline static T Inverse(T value)
		{
			if (value > 0)
			{
				return -value;
			}
			else if (value < 0)
			{
				return Abs(value);
			}
			else
			{
				return value;
			}
		}

		template<typename T>
		constexpr inline static T Sqrt(T value)
		{
			return sqrt(value);
		}

		template<typename T>
		constexpr inline static T Lerp(T from, T to, T amount)
		{
			return from + (to - from) * amount;
		}

		template <typename T>
		constexpr inline static T Atan(T x)
		{
			T result = 0.0;
			T term = x;
			T x_squared = x * x;
			int n = 1;

			for (int i = 0; i < 100; ++i)
			{
				if (i % 2 == 0)
				{
					result += term / n;
				}
				else
				{
					result -= term / n;
				}
				term *= x_squared;
				n += 2;
			}

			return result;
		};

		template <typename T>
		constexpr inline static T Factorical(int n)
		{
			if (n == 0 || n == 1)
			{
				return 1;
			}
			T result = 1;
			for (int i = 2; i <= n; ++i)
			{
				result *= i;
			}
			return result;
		};

		template <typename T>
		constexpr inline static T Asin(T x)
		{
			if (x < -1.0 || x > 1.0)
				return 0;

			T result = x;
			T term = x;
			int n = 1;

			for (int i = 1; i < 100; ++i)
			{
				term *= (x * x * (2 * n - 1) * (2 * n - 1)) / ((2 * n) * (2 * n + 1));
				result += term;
				++n;
			}

			return result;
		};

		template <typename T>
		constexpr inline static T Copysign(T magnitude, T sign)
		{
			return magnitude * (signbit(sign) ? -1.0 : 1.0);
		};

		template <typename T>
		constexpr inline static T Atan2(T x, T y)
		{
			if (x > 0)
			{
				return atan(y / x);
			}
			else if (x < 0 && y >= 0)
			{
				return atan(y / x) + PI;
			}
			else if (x < 0 && y < 0)
			{
				return atan(y / x) - PI;
			}
			else if (x == 0 && y > 0)
			{
				return PI / 2;
			}
			else if (x == 0 && y < 0)
			{
				return -PI / 2;
			}
			else
			{
				return 0;
			}
		};

		template <typename T>
		constexpr inline static T Clamp(T value, T min, T max)
		{
			const T t = value < min ? min : value;
			return t > max ? max : t;
		};
	};
}