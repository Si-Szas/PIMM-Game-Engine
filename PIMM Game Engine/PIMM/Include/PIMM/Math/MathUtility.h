#pragma once
#include <PIMM/Core/Core.h>
#include <numbers>

namespace pimm
{
	namespace MathUtility
	{
		inline auto PI = std::numbers::pi_v<float>;
		inline auto PI2 = std::numbers::pi_v<float> * 2.0f;
		inline f32 ToRadians(f32 degrees) noexcept {
			return degrees * (PI / 180.0f);
		}
	}
}