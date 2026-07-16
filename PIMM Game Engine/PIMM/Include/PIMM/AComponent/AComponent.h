#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/Core/Identifier.h>

namespace pimm 
{
	class AComponent : public Identifier
	{
		pimm_typeid(AComponent)
		
		public:
			//CONSTRUCTOR
			explicit AComponent(const AComponentDescriptor& descriptor);
			
			//GETTER
			AGameObject& GetGameObject() noexcept;

		protected:
			AGameObject& m_object;
			World& m_world;
			GameContext& m_context;
	};
}