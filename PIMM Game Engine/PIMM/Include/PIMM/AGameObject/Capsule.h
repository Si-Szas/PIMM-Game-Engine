#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CapsuleComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>

namespace pimm
{
	class Capsule final : public AGameObject
	{
		pimm_typeid(Capsule)
		public:
			//CONSTRUCTOR
			explicit Capsule(const AGameObjectDescriptor& descriptor);
			
			void OnCreate();
			void OnUpdate(f32 deltaTime);

			//DESTRUCTOR
			~Capsule();

		private:
			CapsuleComponent* m_capsuleComponent{};
			//MaterialComponent* m_materialComponent{};
	};
}
