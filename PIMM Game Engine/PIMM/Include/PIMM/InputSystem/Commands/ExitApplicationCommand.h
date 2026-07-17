#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>

namespace pimm
{
	class ExitApplicationCommand final : public InputCommand
	{
		pimm_typeid(ExitApplicationCommand)	
		public:
			//CONSTRUCTOR
			explicit ExitApplicationCommand(const InputSystemDescriptor& descriptor);

			//FUNCTIONS
			void ExecuteCommand();
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager);
			void UndoCommand(AGameObject& gameObject);

			//DESTRUCTOR
			~ExitApplicationCommand();
	};
}
