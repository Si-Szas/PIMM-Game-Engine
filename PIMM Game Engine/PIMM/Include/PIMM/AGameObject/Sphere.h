#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/SphereComponent.h>

namespace pimm
{
	class Sphere final : public AGameObject
	{
		pimm_typeid(Sphere)
		public:
			//CONSTRUCTOR
			explicit Sphere(const AGameObjectDescriptor& descriptor);

			void OnCreate();
			void OnUpdate(f32 deltaTime);

			//DESTRUCTOR
			~Sphere();

		private:
			SphereComponent* m_sphereComponent{};

			f32 radius = 0.5f;
			ui32 sliceCount = 50 * radius;
			ui32 stackCount = 50 * radius;
	};
}

