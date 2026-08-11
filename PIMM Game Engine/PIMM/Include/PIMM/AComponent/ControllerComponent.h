#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class ControllerComponent final : public AComponent
	{
		pimm_typeid(ControllerComponent)

	public:
		explicit ControllerComponent(const AComponentDescriptor& descriptor);

		void Update(f32 deltaTime);

		void SetSensitivity(f32 sensitivity) noexcept { m_sensitivity = sensitivity; }
		f32 GetSensitivity() const noexcept { return m_sensitivity; }
		void SetSpeedModifier(f32 speed) noexcept { m_speedModifier = speed; }
		f32 GetSpeedModifier() const noexcept { return m_speedModifier; }

	private:
		f32 m_sensitivity = 0.1f;
		f32 m_speedModifier = 3.0f;
	};
}
