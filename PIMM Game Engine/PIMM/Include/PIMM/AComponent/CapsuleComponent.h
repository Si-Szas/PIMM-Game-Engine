#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/CylinderComponent.h>

namespace pimm {
	class CapsuleComponent final : public AComponent
	{
		pimm_typeid(CapsuleComponent)
		public:
			//CONSTRUCTOR
			explicit CapsuleComponent(const AComponentDescriptor& descriptor);
	};
}
