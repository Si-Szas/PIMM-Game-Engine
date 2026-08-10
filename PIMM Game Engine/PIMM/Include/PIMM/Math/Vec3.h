#pragma once
#include <PIMM/Core/Core.h>
#include <cmath>

namespace pimm
{
	class Vec3
	{
	public:
		//CONSTRUCTOR
		Vec3() = default;
		Vec3(f32 all) : x(all), y(all), z(all)
		{
		}
		Vec3(f32 _x, f32 _y, f32 _z) : x(_x), y(_y), z(_z)
		{
		}

		Vec3 operator+=(const Vec3& rightHandSide)
		{
			x += rightHandSide.x;
			y += rightHandSide.y;
			z += rightHandSide.z;

			return *this;
		}

		Vec3 operator-=(const Vec3& rightHandSide)
		{
			x -= rightHandSide.x;
			y -= rightHandSide.y;
			z -= rightHandSide.z;

			return *this;
		}

		Vec3 operator*=(const Vec3& rightHandSide)
		{
			x *= rightHandSide.x;
			y *= rightHandSide.y;
			z *= rightHandSide.z;

			return *this;
		}

		static Vec3 Normalize(const Vec3& vector)
		{
			float lengthSquared = LengthSquared(vector);

			if (lengthSquared == 0) return Vec3{ 0.0f };

			float inverseLength = 1.0f / std::sqrt(lengthSquared);

			return Vec3
			{
				vector.x * inverseLength,
				vector.y * inverseLength,
				vector.z * inverseLength
			};
		}

		static float LengthSquared(const Vec3& vector)
		{
			return
				vector.x * vector.x +
				vector.y * vector.y +
				vector.z * vector.z;
		}

		static Vec3 Lerp(const Vec3& vector1, const Vec3& vector2, float t)
		{
			//Clamp the lerp value
			if (t >= 1.0f) t = 1.0f;
			else if (t <= 0.0f) t = 0.0f;
			//Linear interpolation formula
			return vector1 + (vector2 - vector1) * t;
		}

		friend Vec3 operator+(Vec3 leftHandSide, const Vec3& rightHandSide)
		{
			leftHandSide += rightHandSide;
			return leftHandSide;
		}

		friend Vec3 operator-(Vec3 leftHandSide, const Vec3& rightHandSide)
		{
			leftHandSide -= rightHandSide;
			return leftHandSide;
		}

		friend Vec3 operator/(Vec3 leftHandSide, float scalar)
		{
			float reciprocal = 1.0f / scalar;
			return leftHandSide * reciprocal;
		}

		friend Vec3 operator*(Vec3 vector, f32 scalar)
		{
			vector *= scalar;
			return vector;
		}

		friend Vec3 operator*(f32 scalar, Vec3 vector)
		{
			vector *= scalar;
			return vector;
		}

	public:
		f32 x{}, y{}, z{};
	};
}