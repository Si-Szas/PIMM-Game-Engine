#include <PIMM/AGameObject/Capsule.h>

pimm::Capsule::Capsule(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Capsule::OnCreate()
{
    m_capsuleComponent = CreateOrGetComponent<CapsuleComponent>();
    //m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Capsule::OnUpdate(f32 deltaTime)
{

}

pimm::Capsule::~Capsule()
{

}