#include <PIMM/AGameObject/MeshObject.h>

pimm::MeshObject::MeshObject(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::MeshObject::OnCreate()
{
	m_name = "Mesh";
	m_meshComponent = CreateOrGetComponent<MeshComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::MeshObject::OnUpdate(f32 deltaTime) {

}

pimm::MeshObject::~MeshObject() {

}