#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	class MoveLeftCommand final : public InputCommand
	{
		pimm_typeid(MoveLeftCommand)
		public:
			//CONSTRUCTOR
			explicit MoveLeftCommand(const InputSystemDescriptor& descriptor);
			//FUNCTIONS
			void ExecuteCommand() {}
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager);
			void UndoCommand(AGameObject& gameObject);

			~MoveLeftCommand();

		private:
			Vec3 m_previousPosition{};
	};
}