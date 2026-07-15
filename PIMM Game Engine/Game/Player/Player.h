#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CameraComponent.h>

namespace pimm
{
	class Player : public AGameObject
	{
		pimm_typeid(Player)
		public:
			//CONSTRUCTOR
			explicit Player(const AGameObjectDescriptor& descriptor);

			//DESTRUCTOR
			virtual ~Player() override;
		
		protected:
			virtual void OnCreate();
			virtual void OnUpdate(f32 deltaTime);

		private:
			CameraComponent* m_camera{};
	};
}

