#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>
#include <PIMM/Game/World.h>
#include <PIMM/Game/WorldRenderer.h>

pimm::AGameObject::AGameObject(const AGameObjectDescriptor& descriptor) :
	Identifier(descriptor.base),
	m_gameContext(descriptor.gameContext),
	m_world(descriptor.world),
	m_worldRenderer(descriptor.worldRenderer)
{
	m_transform = CreateOrGetComponent<TransformComponent>();
	m_material = CreateOrGetComponent<MaterialComponent>();
}

pimm::AGameObject::~AGameObject()
{
}

pimm::AComponent* pimm::AGameObject::CreateComponentInternal(UniquePtr<AComponent>& component)
{
	if (component)
	{
		auto typeID = component->GetTypeID();
		auto pointer = component.get();

		if (m_components.find(typeID) != m_components.end()) return {};
		
		m_components.emplace(typeID, std::move(component));
		m_world.AddComponentInternal(*pointer);

		return pointer;
	}

	return {};
}

pimm::AComponent* pimm::AGameObject::GetComponentInternal(size_t ID)
{
	auto it = m_components.find(ID);
	
	if (it != m_components.end()) return it->second.get();
	
	return {};
}

//pimm::ui32 pimm::AGameObject::GetVertexOffset() noexcept
//{
//	return m_vertexOffset;
//}
//
//void pimm::AGameObject::SetVertexOffset(ui32 newOffset) noexcept
//{
//	m_vertexOffset = newOffset;
//}
//
//pimm::ui32 pimm::AGameObject::GetIndexLocation() noexcept
//{
//	return m_indexLocation;
//}
//
//void pimm::AGameObject::SetIndexLocation(ui32 newOffset) noexcept
//{
//	m_indexLocation = newOffset;
//}

pimm::TransformComponent& pimm::AGameObject::GetTransform() noexcept
{
	return *m_transform;
}

pimm::World& pimm::AGameObject::GetWorld() noexcept
{
	return m_world;
}

pimm::InputSystem& pimm::AGameObject::GetInputSystem() noexcept
{
	return m_gameContext.inputSystem;
}

const pimm::WorldRenderer& pimm::AGameObject::GetWorldRenderer() noexcept
{
	return m_worldRenderer;
}

pimm::ResourceManager& pimm::AGameObject::GetResourceManager() noexcept
{
	return m_gameContext.resourceManager;
}

pimm::MaterialComponent& pimm::AGameObject::GetMaterialComponent() noexcept
{
	return *m_material;
}

void pimm::AGameObject::ResetMovementModifiers()
{
	forwardModifier = 0.0f;
	rightModifier = 0.0f;
}

pimm::Vec3 pimm::AGameObject::GetVelocity() const noexcept
{
	return m_velocity;
}

void pimm::AGameObject::SetVelocity(Vec3 newVelocity) noexcept
{
	m_velocity = newVelocity;
}

pimm::f32 pimm::AGameObject::GetForwardModifier() const noexcept
{
	return forwardModifier;
}

void pimm::AGameObject::SetForwardModifier(f32 newForward) noexcept
{
	forwardModifier = newForward;
}

pimm::f32 pimm::AGameObject::GetRightModifier() const noexcept
{
	return rightModifier;
}

void pimm::AGameObject::SetRightModifier(f32 newRight) noexcept
{
	rightModifier = newRight;
}

pimm::f32 pimm::AGameObject::GetSpeedModifier() const noexcept
{
	return speedModifier;
}

void pimm::AGameObject::SetSpeedModifier(f32 newSpeed) noexcept
{
	speedModifier = newSpeed;
}

//size_t pimm::AGameObject::GetWorldIndex() const noexcept
//{
//	return m_worldIndex;
//}
//
//void pimm::AGameObject::SetWorldIndex(size_t index) noexcept
//{
//	m_worldIndex = index;
//}
pimm::ui32 pimm::AGameObject::GetVertexOffset() noexcept
{
	return m_vertexOffset;
}

void pimm::AGameObject::SetVertexOffset(ui32 newOffset) noexcept
{
	m_vertexOffset = newOffset;
}

pimm::ui32 pimm::AGameObject::GetIndexLocation() noexcept
{
	return m_indexLocation;
}

void pimm::AGameObject::SetIndexLocation(ui32 newLocation) noexcept
{
	m_indexLocation = newLocation;
}