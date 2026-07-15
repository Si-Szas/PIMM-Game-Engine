#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Identifier.h>
#include <PIMM/AGameObject/AGameObject.h>

namespace pimm
{
	class InputCommand : public Identifier
	{
		pimm_typeid(InputCommand)
		public:
			//CONSTRUCTOR
			explicit InputCommand(const InputSystemDescriptor& descriptor);

			//FUNCTION
			virtual void ExecuteCommand() = 0;
			virtual void ExecuteCommand(AGameObject& gameObject, World& world) = 0;
			virtual void UndoCommand(AGameObject& gameObject) = 0;
			//virtual void RedoCommand(AGameObject& gameObject) = 0;

			//DESTRUCTOR
			virtual ~InputCommand();
		protected:
			BaseDescriptor m_base;
			World* m_world;
	};
}