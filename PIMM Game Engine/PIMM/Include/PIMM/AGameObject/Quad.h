#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/QuadComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>

namespace pimm
{
	class Quad final : public AGameObject
	{
		pimm_typeid(Quad)
		public:
			//CONSTRUCTOR
			explicit Quad(const AGameObjectDescriptor& descriptor);

			void OnCreate();
			void OnUpdate(f32 deltaTime);

			//DESTRUCTOR
			~Quad();

		private:
			QuadComponent* m_quadComponent{};
			//MaterialComponent* m_materialComponent{};
	};
}