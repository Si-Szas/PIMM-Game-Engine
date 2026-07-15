#pragma once
#include <PIMM/Core/Core.h>

namespace pimm
{
	class Vec4
	{
		public:
			//CONSTRUCTOR
			Vec4() = default;
			Vec4(f32 all) : x(all), y(all), z(all), w(all)
			{
			}
			Vec4(f32 _x, f32 _y, f32 _z, f32 _w) : x(_x), y(_y), z(_z), w(_w)
			{
			}

			static Vec4 Cross(Vec4& v1, Vec4& v2, Vec4& v3) noexcept
			{
				return {
					 v1.y * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.y * v3.w - v3.y * v2.w) + v1.w * (v2.y * v3.z - v2.z * v3.y),
					 -(v1.x * (v2.z * v3.w - v3.z * v2.w) - v1.z * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.z - v3.x * v2.z)),
					 v1.x * (v2.y * v3.w - v3.y * v2.w) - v1.y * (v2.x * v3.w - v3.x * v2.w) + v1.w * (v2.x * v3.y - v3.x * v2.y),
					 -(v1.x * (v2.y * v3.z - v3.y * v2.z) - v1.y * (v2.x * v3.z - v3.x * v2.z) + v1.z * (v2.x * v3.y - v3.x * v2.y))
				};
			}

		public:
			f32 x{}, y{}, z{}, w{};
	};
}