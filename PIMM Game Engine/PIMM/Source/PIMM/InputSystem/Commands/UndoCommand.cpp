#include <PIMM/InputSystem/Commands/UndoCommand.h>


pimm::UndoCommand::UndoCommand(const InputSystemDescriptor& descriptor) : InputCommand(descriptor)
{
}

void pimm::UndoCommand::ExecuteCommand(AGameObject& gameObject)
{
	if (m_snapshot != nullptr)
		gameObject.restoreFromSnapshot(*m_snapshot);
}

void pimm::UndoCommand::ExecuteCommand()
{
}

void pimm::UndoCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	if (m_snapshot != nullptr)
		world.GetSelectedGameObject()->restoreFromSnapshot(*m_snapshot);
}

void pimm::UndoCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{
}

void pimm::UndoCommand::retrieveSnapshot()
{
	*m_snapshot = *m_world->GetSelectedGameObject()->saveToSnapshot();
}

pimm::UndoCommand::~UndoCommand()
{
}

