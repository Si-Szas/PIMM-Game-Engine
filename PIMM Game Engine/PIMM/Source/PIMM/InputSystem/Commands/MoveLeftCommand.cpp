#include <PIMM/InputSystem/Commands/MoveLeftCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>

pimm::MoveLeftCommand::MoveLeftCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::MoveLeftCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	m_previousPosition = gameObject.GetTransform().GetPosition();

	gameObject.SetRightModifier(gameObject.GetRightModifier() - 1.0f);
}

void pimm::MoveLeftCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

void pimm::MoveLeftCommand::UndoCommand(AGameObject& gameObject)
{
	gameObject.GetTransform().SetPosition(m_previousPosition);
}

pimm::MoveLeftCommand::~MoveLeftCommand()
{
}

