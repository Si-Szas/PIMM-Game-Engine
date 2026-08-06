#include <PIMM/AGameObject/Cylinder.h>

pimm::Cylinder::Cylinder(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Cylinder::OnCreate()
{
    m_cylinderComponent = CreateOrGetComponent<CylinderComponent>();
    //m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Cylinder::OnUpdate(f32 deltaTime)
{

}

pimm::Cylinder::~Cylinder()
{

}
