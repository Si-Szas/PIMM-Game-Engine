#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CylinderComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>

namespace pimm
{
	class Cylinder final : public AGameObject
	{
		pimm_typeid(Cylinder)
		public:
			//CONSTRUCTOR
			explicit Cylinder(const AGameObjectDescriptor& descriptor);

			void OnCreate();
			void OnUpdate(f32 deltaTime);

			~Cylinder();

		private:
			CylinderComponent* m_cylinderComponent{};
			//MaterialComponent* m_materialComponent{};
	};
}