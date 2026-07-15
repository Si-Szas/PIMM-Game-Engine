#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class QuadComponent final : public AComponent
	{
		pimm_typeid(QuadComponent)
		public:
			//CONSTRUCTOR
			explicit QuadComponent(const AComponentDescriptor& descriptor);
	};
}
