#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class CylinderComponent final : public AComponent
	{
		pimm_typeid(CylinderComponent)
		public:
			//CONSTRUCTOR
			explicit CylinderComponent(const AComponentDescriptor& descriptor); 
	};
}