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

		public:
			f32 x{}, y{};
	};
}