#include <PIMM/AGameObject/Quad.h>

pimm::Quad::Quad(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Quad::OnCreate() 
{
	m_quadComponent = CreateOrGetComponent<QuadComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Quad::OnUpdate(f32 deltaTime) {

}

pimm::Quad::~Quad() {

}