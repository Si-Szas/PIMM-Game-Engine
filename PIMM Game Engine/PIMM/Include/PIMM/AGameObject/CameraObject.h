#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CameraComponent.h>

namespace pimm
{
	class CameraObject final : public AGameObject
	{
		pimm_typeid(CameraObject)

	public:
		explicit CameraObject(const AGameObjectDescriptor& descriptor);

		void OnCreate();
		void OnUpdate(f32 deltaTime);

		~CameraObject();

	private:
		CameraComponent* m_cameraComponent{};
	};
}
