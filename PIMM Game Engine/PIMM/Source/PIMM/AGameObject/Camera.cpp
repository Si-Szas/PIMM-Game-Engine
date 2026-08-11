#include <PIMM/AGameObject/Camera.h>

pimm::Camera::Camera(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Camera::OnCreate()
{
	m_name = "Camera";
	m_cameraComponent = CreateOrGetComponent<CameraComponent>();
	m_cameraComponent->SetGameCamera(true);
}

void pimm::Camera::OnUpdate(f32 deltaTime)
{
}

pimm::Camera::~Camera()
{
}
