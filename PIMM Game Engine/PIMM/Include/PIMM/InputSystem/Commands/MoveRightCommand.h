#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	class MoveRightCommand final : public InputCommand
	{
		pimm_typeid(MoveRightCommand)
		public:
			//CONSTRUCTOR
			explicit MoveRightCommand(const InputSystemDescriptor& descriptor);
			//FUNCTIONS
			void ExecuteCommand() {}
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager);
			void UndoCommand(AGameObject& gameObject);

			~MoveRightCommand();

		private:
			Vec3 m_previousPosition{};
	};
}