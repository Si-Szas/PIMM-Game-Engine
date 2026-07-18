#include <PIMM/InputSystem/Commands/MoveBackwardCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>

pimm::MoveBackwardCommand::MoveBackwardCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::MoveBackwardCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	m_previousPosition = gameObject.GetTransform().GetPosition();

	gameObject.SetForwardModifier(gameObject.GetForwardModifier() - 1.0f);
}

void pimm::MoveBackwardCommand::UndoCommand(AGameObject& gameObject)
{
	gameObject.GetTransform().SetPosition(m_previousPosition);
}

void pimm::MoveBackwardCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

pimm::MoveBackwardCommand::~MoveBackwardCommand()
{
}
