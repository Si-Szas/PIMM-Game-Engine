#include <PIMM/AGameObject/Cube.h>

pimm::Cube::Cube(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Cube::OnCreate() 
{
	m_cubeComponent = CreateOrGetComponent<CubeComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Cube::OnUpdate(f32 deltaTime)
{
}

pimm::Cube::~Cube()
{
}
