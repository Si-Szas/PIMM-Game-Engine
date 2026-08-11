#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Game/World.h>
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	class UndoCommand : public InputCommand
	{
		pimm_typeid(UndoCommand)
	public:
		explicit UndoCommand(const InputSystemDescriptor& descriptor);

		void ExecuteCommand(AGameObject& gameObject);

		void ExecuteCommand();
		void ExecuteCommand(AGameObject& gameObject, World& world);
		void ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager);

		void retrieveSnapshot();

		~UndoCommand();

	private:
		
		AGameObject::Snapshot* m_snapshot;


	};
}

