#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/Game/World.h>

using namespace pimm;

pimm::TransformComponent::TransformComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
	MarkAsDirty();
}

void pimm::TransformComponent::SetPosition(const Vec3& position)
{
	m_position = position;
	MarkAsDirty();
}

Vec3 pimm::TransformComponent::GetPosition() const noexcept
{
	return m_position;
}

void pimm::TransformComponent::SetScale(const Vec3& scale)
{
	m_scale = scale;
	MarkAsDirty();
}

Vec3 pimm::TransformComponent::GetScale() const noexcept
{
	return m_scale;
}

void pimm::TransformComponent::SetRotation(const Vec3& rotation)
{
	m_rotation = rotation;
	MarkAsDirty();
}

Vec3 pimm::TransformComponent::GetRotation() const noexcept
{
	return m_rotation;
}

Vec3 pimm::TransformComponent::Forward()
{
	auto forward = GetRigidWorldMatrix().Row(2);
	return pimm::Vec3::Normalize({ forward.x, forward.y, forward.z });
}

Vec3 pimm::TransformComponent::Right()
{
	auto right = GetRigidWorldMatrix().Row(0);
	return pimm::Vec3::Normalize({ right.x, right.y, right.z });
}

Vec3 pimm::TransformComponent::Up()
{
	auto up = GetRigidWorldMatrix().Row(1);
	return pimm::Vec3::Normalize({ up.x, up.y, up.z });
}

void pimm::TransformComponent::UpdateWorldMatrix() noexcept
{
	//If the transform component is not marked to be updated, then return
	if (m_isDirty)
	{
		m_isDirty = false;

		m_rigidWorldMatrix =
			//Concerned only about rotation and translate
			Matrix4x4::RotateAlongX(m_rotation.x) *
			Matrix4x4::RotateAlongY(m_rotation.y) *
			Matrix4x4::RotateAlongZ(m_rotation.z) *

			//Then translate
			Matrix4x4::Translate(m_position);

		m_affineWorldMatrix =
			Matrix4x4::Scale(m_scale) * m_rigidWorldMatrix;
	}
}

Matrix4x4 pimm::TransformComponent::GetAffineWorldMatrix() noexcept
{
	UpdateWorldMatrix();
	return m_affineWorldMatrix;
}

Matrix4x4 pimm::TransformComponent::GetRigidWorldMatrix() noexcept
{
	UpdateWorldMatrix();
	return m_rigidWorldMatrix;
}

void pimm::TransformComponent::MarkAsDirty()
{
	//If its already marked as dirty, then return
	if (!m_isDirty)
	{
		m_isDirty = true;
		m_world.AddDirtyTransformInternal(*this);
	}
}
