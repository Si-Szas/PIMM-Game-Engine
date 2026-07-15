#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>

namespace pimm
{
	class DeleteAGameObjectCommand final : public InputCommand
	{
		pimm_typeid(DeleteAGameObjectCommand)
		public:
			//CONSTRUCTOR
			explicit DeleteAGameObjectCommand(const InputSystemDescriptor& descriptor);
			
			//FUNCTIONS
			void ExecuteCommand();
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void UndoCommand(AGameObject& gameObject);

			//DESTRUCTOR
			~DeleteAGameObjectCommand();
	};
}
