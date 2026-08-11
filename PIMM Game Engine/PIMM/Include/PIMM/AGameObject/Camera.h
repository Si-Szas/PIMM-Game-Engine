#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CameraComponent.h>

namespace pimm
{
	class Camera : public AGameObject
	{
		pimm_typeid(Camera)
	public:
		explicit Camera(const AGameObjectDescriptor& descriptor);

		void OnCreate();
		void OnUpdate(f32 deltaTime);

		~Camera();

	private:
		CameraComponent* m_cameraComponent{};
	};
}
