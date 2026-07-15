#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class MaterialComponent final : public AComponent
	{
		pimm_typeid(MaterialComponent)

		//CONSTRUCTOR
		explicit MaterialComponent(const AComponentDescriptor& descriptor);

		//SETTERS
		void SetMaterial(const RefPtr<MaterialResource>& material);
		MaterialResource* GetMaterial();

	private:
		RefPtr<MaterialResource> m_material{};
	};
}