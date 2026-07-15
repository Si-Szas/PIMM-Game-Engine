#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>

pimm::CameraComponent::CameraComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
	ComputeProjectionMatrix();
}

pimm::Matrix4x4 pimm::CameraComponent::GetViewMatrix() noexcept
{
	return Matrix4x4::Inverse(m_object.GetTransform().GetRigidWorldMatrix());
}

pimm::Matrix4x4 pimm::CameraComponent::GetProjectionMatrix() const noexcept
{
	return m_projection;
}

void pimm::CameraComponent::SetFarPlane(f32 farPlane) noexcept
{
	if (farPlane <= m_nearPlane) return;
	m_farPlane = farPlane;
	ComputeProjectionMatrix();
}

pimm::f32 pimm::CameraComponent::GetFarPlane() const noexcept
{
	return m_farPlane;
}

void pimm::CameraComponent::SetNearPlane(f32 nearPlane) noexcept
{
	if (nearPlane <= 0.001f) return;
	m_nearPlane = nearPlane;
	ComputeProjectionMatrix();
}

pimm::f32 pimm::CameraComponent::GetNearPlane() const noexcept
{
	return m_nearPlane;
}

void pimm::CameraComponent::SetFieldOfView(f32 fieldOfView) noexcept
{
	if (fieldOfView <= 0.001f || fieldOfView >= MathUtility::PI) return;
	m_fieldOfView = fieldOfView;
	ComputeProjectionMatrix();
}

pimm::f32 pimm::CameraComponent::GetFieldOfView() const noexcept
{
	return m_fieldOfView;
}

void pimm::CameraComponent::SetViewportSize(const Rect& size) noexcept
{
	if (m_viewportSize == size) return;
	if (m_viewportSize.width == 0 || m_viewportSize.height == 0) return;

	m_viewportSize = size;
	ComputeProjectionMatrix();
}

pimm::Rect pimm::CameraComponent::GetViewportSize() const noexcept
{
	return m_viewportSize;
}

void pimm::CameraComponent::ComputeProjectionMatrix() noexcept
{
	m_projection = 
		Matrix4x4::PerspectiveFOVLH
		(
			m_fieldOfView, 
			(f32)m_viewportSize.width / (f32)m_viewportSize.height,
			m_nearPlane, m_farPlane
		);
}

