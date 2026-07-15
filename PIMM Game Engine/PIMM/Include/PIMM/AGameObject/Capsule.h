#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CapsuleComponent.h>

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

			f32 height = 1.0f;
			f32 radius = 0.5f;
			ui32 sliceCount = 50 * radius;
			ui32 stackCount = 10 * height;
	};
}
