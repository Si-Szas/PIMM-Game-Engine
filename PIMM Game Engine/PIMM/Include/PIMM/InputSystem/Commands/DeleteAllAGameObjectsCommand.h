#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>

namespace pimm
{
	class DeleteAllAGameObjectsCommand final : public InputCommand
	{
		pimm_typeid(DeleteAllAGameObjectsCommand)
		public:
			//CONSTRUCTOR
			explicit DeleteAllAGameObjectsCommand(const InputSystemDescriptor& descriptor);

			//FUNCTIONS
			void ExecuteCommand();
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void UndoCommand(AGameObject& gameObject);

			~DeleteAllAGameObjectsCommand();
	};
}
