#include <PIMM/AComponent/MaterialComponent.h>


pimm::MaterialComponent::MaterialComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
}

void pimm::MaterialComponent::SetMaterial(const RefPtr<MaterialResource>& material)
{
	m_material = material;
}

pimm::MaterialResource* pimm::MaterialComponent::GetMaterial()
{
	return m_material.get();
}
