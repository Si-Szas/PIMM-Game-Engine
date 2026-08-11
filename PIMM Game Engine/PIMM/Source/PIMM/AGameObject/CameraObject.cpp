#include <PIMM/AGameObject/CameraObject.h>
#include <PIMM/Game/World.h>

pimm::CameraObject::CameraObject(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::CameraObject::OnCreate()
{
	m_name = "Camera";
	m_cameraComponent = CreateOrGetComponent<CameraComponent>();
	if (!GetWorld().HasActiveCameraObject())
		m_cameraComponent->SetActive(true);
}

void pimm::CameraObject::OnUpdate(f32 deltaTime)
{
	auto* controller = GetComponent<ControllerComponent>();
	if (controller)
		controller->Update(deltaTime);
}

bool pimm::CameraObject::HasController() noexcept
{
	return GetComponent<ControllerComponent>() != nullptr;
}

pimm::CameraObject::~CameraObject()
{
}
