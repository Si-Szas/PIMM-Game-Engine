#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/SphereComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>

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
			//MaterialComponent* m_materialComponent{};
	};
}

