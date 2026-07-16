#include <PIMM/AComponent/AComponent.h>

pimm::AComponent::AComponent(const AComponentDescriptor& descriptor) :
	Identifier(descriptor.base),
	m_object(descriptor.object),
	m_world(descriptor.world),
	m_context(descriptor.gameContext)
{
}

pimm::AGameObject& pimm::AComponent::GetGameObject() noexcept
{
	return m_object;
}
