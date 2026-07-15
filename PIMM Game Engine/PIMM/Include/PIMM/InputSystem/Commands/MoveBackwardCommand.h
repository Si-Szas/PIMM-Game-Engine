#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	class MoveBackwardCommand final : public InputCommand
	{
		pimm_typeid(MoveBackwardCommand)
		public:
			//CONSTRUCTOR
			explicit MoveBackwardCommand(const InputSystemDescriptor& descriptor);
			//FUNCTIONS
			void ExecuteCommand(){}
			void ExecuteCommand(AGameObject& gameObject, World& world);
			void UndoCommand(AGameObject& gameObject);

			~MoveBackwardCommand();
		
		private:
			Vec3 m_previousPosition{};
	};
}