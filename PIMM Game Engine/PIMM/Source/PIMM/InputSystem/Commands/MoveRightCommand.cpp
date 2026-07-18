#include <PIMM/InputSystem/Commands/MoveRightCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>

pimm::MoveRightCommand::MoveRightCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::MoveRightCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	m_previousPosition = gameObject.GetTransform().GetPosition();

	gameObject.SetRightModifier(gameObject.GetRightModifier() + 1.0f);
}

void pimm::MoveRightCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

void pimm::MoveRightCommand::UndoCommand(AGameObject& gameObject)
{
	gameObject.GetTransform().SetPosition(m_previousPosition);
}

pimm::MoveRightCommand::~MoveRightCommand()
{
}
