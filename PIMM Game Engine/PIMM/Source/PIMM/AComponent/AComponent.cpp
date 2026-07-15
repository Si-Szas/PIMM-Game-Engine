#include <PIMM/AComponent/AComponent.h>

pimm::AComponent::AComponent(const AComponentDescriptor& descriptor) :
	Identifier(descriptor.base),
	m_object(descriptor.object),
	m_world(descriptor.world)
{
}

pimm::AGameObject& pimm::AComponent::GetGameObject() noexcept
{
	return m_object;
}
