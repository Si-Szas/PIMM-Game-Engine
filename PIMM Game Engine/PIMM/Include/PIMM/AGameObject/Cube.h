#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/CubeComponent.h>

namespace pimm 
{
	class Cube final : public AGameObject
	{
		pimm_typeid(Cube)
		public:
			//CONSTRUCTOR
			explicit Cube(const AGameObjectDescriptor& descriptor);
			
			void OnCreate();
			void OnUpdate(f32 deltaTime);

			//DESTRUCTOR
			~Cube();

		private:
			CubeComponent* m_cubeComponent{};
	};
}

