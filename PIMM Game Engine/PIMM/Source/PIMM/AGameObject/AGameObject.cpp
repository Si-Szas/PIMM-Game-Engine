#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>
#include <PIMM/Game/World.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Math/Matrix4x4.h>
#include <PIMM/Math/MathUtility.h>
#include <cmath>

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

const std::unordered_map<size_t, pimm::UniquePtr<pimm::AComponent>>& pimm::AGameObject::GetAllComponents() const noexcept
{
	return m_components;
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

pimm::GraphicsDevice& pimm::AGameObject::GetGraphicsDevice() noexcept
{
	return m_gameContext.graphicsDevice;
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

const std::string& pimm::AGameObject::GetObjectName() noexcept
{
	return m_name;
}

const char* pimm::AGameObject::GetObjectLabel(AGameObject* object)
{
	if (!object) return "Game Object";
	return object->m_name.c_str();
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

pimm::AGameObject* pimm::AGameObject::GetParent() const noexcept
{
	return m_parent;
}

const std::vector<pimm::AGameObject*>& pimm::AGameObject::GetChildren() const noexcept
{
	return m_children;
}

namespace
{
	// Decomposes an affine matrix built the way pimm::TransformComponent builds them
	// (rows 0-2 = scaled rotation basis vectors, row 3 = translation) back into
	// position / rotation (degrees, matching the RotX*RotY*RotZ order used to build it) / scale.
	void DecomposeAffine(const pimm::Matrix4x4& m, pimm::Vec3& outPosition, pimm::Vec3& outRotationDegrees, pimm::Vec3& outScale)
	{
		using namespace pimm;

		Vec4 row0 = m.Row(0);
		Vec4 row1 = m.Row(1);
		Vec4 row2 = m.Row(2);
		Vec4 row3 = m.Row(3);

		outPosition = Vec3{ row3.x, row3.y, row3.z };

		Vec3 basisX{ row0.x, row0.y, row0.z };
		Vec3 basisY{ row1.x, row1.y, row1.z };
		Vec3 basisZ{ row2.x, row2.y, row2.z };

		auto length = [](const Vec3& v) noexcept
			{
				return std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
			};

		f32 scaleX = length(basisX);
		f32 scaleY = length(basisY);
		f32 scaleZ = length(basisZ);

		outScale = Vec3{ scaleX, scaleY, scaleZ };

		Vec3 r0 = (scaleX > 0.0001f) ? Vec3{ basisX.x / scaleX, basisX.y / scaleX, basisX.z / scaleX } : Vec3{ 1.0f, 0.0f, 0.0f };
		Vec3 r1 = (scaleY > 0.0001f) ? Vec3{ basisY.x / scaleY, basisY.y / scaleY, basisY.z / scaleY } : Vec3{ 0.0f, 1.0f, 0.0f };
		Vec3 r2 = (scaleZ > 0.0001f) ? Vec3{ basisZ.x / scaleZ, basisZ.y / scaleZ, basisZ.z / scaleZ } : Vec3{ 0.0f, 0.0f, 1.0f };

		f32 sy = -r0.z;
		if (sy > 1.0f) sy = 1.0f;
		if (sy < -1.0f) sy = -1.0f;

		f32 yRad = std::asin(sy);
		f32 xRad = std::atan2(r1.z, r2.z);
		f32 zRad = std::atan2(r0.y, r0.x);

		const f32 radToDeg = 180.0f / MathUtility::PI;

		outRotationDegrees = Vec3{ xRad * radToDeg, yRad * radToDeg, zRad * radToDeg };
	}
}

void pimm::AGameObject::SetParent(AGameObject* newParent)
{
	if (newParent == this) return;
	if (newParent == m_parent) return;

	for (AGameObject* p = newParent; p != nullptr; p = p->GetParent())
	{
		if (p == this) return;
	}
	Matrix4x4 oldWorldMatrix = GetTransform().GetAffineWorldMatrix();

	if (m_parent)
	{
		m_parent->RemoveChildInternal(this);
	}

	m_parent = newParent;

	if (m_parent)
	{
		m_parent->AddChildInternal(this);
	}

	Matrix4x4 newLocalMatrix;
	if (m_parent)
	{
		Matrix4x4 newParentWorldMatrix = m_parent->GetTransform().GetAffineWorldMatrix();
		Matrix4x4 inverseNewParentWorld = Matrix4x4::Inverse(newParentWorldMatrix);
		newLocalMatrix = oldWorldMatrix * inverseNewParentWorld;
	}
	else
	{
		newLocalMatrix = oldWorldMatrix;
	}

	Vec3 position, rotationDegrees, scale;
	DecomposeAffine(newLocalMatrix, position, rotationDegrees, scale);

	GetTransform().SetPosition(position);
	GetTransform().SetRotation(rotationDegrees);
	GetTransform().SetScale(scale);

	// World matrix now depends on a different parent chain.
	GetTransform().MarkAsDirty();
}

void pimm::AGameObject::AddChildInternal(AGameObject* child)
{
	m_children.push_back(child);
}

void pimm::AGameObject::RemoveChildInternal(AGameObject* child)
{
	std::erase(m_children, child); 
}