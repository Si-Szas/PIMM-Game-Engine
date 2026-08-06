#pragma once

namespace pimm 
{
	class Vec2
	{
		public:
			//CONSTRUCTOR
			Vec2() = default;
			Vec2(f32 all) : x(all), y(all)
			{
			}
			Vec2(f32 _x, f32 _y) : x(_x), y(_y)
			{
			}

			friend Vec2 operator+(Vec2 leftHandSide, const Vec2& rightHandSide) {
				leftHandSide.x += rightHandSide.x;
				leftHandSide.y += rightHandSide.y;
				return leftHandSide;
			}

			friend Vec2 operator*(Vec2 leftHandSide, float scalar) {
				leftHandSide.x *= scalar;
				leftHandSide.y *= scalar;
				return leftHandSide;
			}

			friend Vec2 operator/(Vec2 leftHandSide, float scalar)
			{
				float reciprocal = 1.0f / scalar;
				return leftHandSide * reciprocal;
			}

		public:
			f32 x{}, y{};
	};
}