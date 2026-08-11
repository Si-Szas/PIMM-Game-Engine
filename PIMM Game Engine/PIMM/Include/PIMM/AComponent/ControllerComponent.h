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

	private:
		f32 m_sensitivity = 0.1f;
		f32 m_speedModifier = 3.0f;
	};
}
