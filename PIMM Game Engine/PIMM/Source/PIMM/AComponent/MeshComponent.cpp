#include <PIMM/AComponent/MeshComponent.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Game/World.h>
#include <PIMM/Game/Game.h>
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Resource/MeshResource.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::MeshComponent::MeshComponent(const AComponentDescriptor& data) :
	AComponent(data)
{
}

pimm::MeshResource* pimm::MeshComponent::GetMesh() const noexcept
{
	return m_mesh.get();
}

pimm::MaterialResource* pimm::MeshComponent::GetMaterial(ui32 index) const noexcept
{
	if (index >= m_materials.size())
	{
		PIMMLogError("Index {} is out of bounds for material list (size: {})", index, m_materials.size());

		return {};
	}

	return m_materials[index].get();
}

void pimm::MeshComponent::SetMesh(const RefPtr<MeshResource>& newMesh)
{
	m_mesh = newMesh;

	if (m_mesh) m_materials.resize(m_mesh->GetNumberOfMaterialSlots());
	else m_materials.resize(0);
}

void pimm::MeshComponent::SetMaterial(ui32 index, const RefPtr<MaterialResource>& newMaterial)
{
	if (index >= m_materials.size())
	{
		PIMMLogError("Index {} is out of bounds for material list (size: {}). Ensure to SetMesh() before setting materials.", index, m_materials.size());

		return;
	}

	m_materials[index] = newMaterial;
}
