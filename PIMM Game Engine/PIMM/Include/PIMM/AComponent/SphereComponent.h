#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class SphereComponent final : public AComponent
	{
		pimm_typeid(SphereComponent)
		public:
			//CONSTRUCTOR
			explicit SphereComponent(const AComponentDescriptor& descriptor);
	};
}

