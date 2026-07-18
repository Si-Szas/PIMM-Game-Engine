#include <PIMM/InputSystem/Commands/MoveForwardCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>

#include <iostream>

pimm::MoveForwardCommand::MoveForwardCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::MoveForwardCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	m_previousPosition = gameObject.GetTransform().GetPosition();

	gameObject.SetForwardModifier(gameObject.GetForwardModifier() + 1.0f);
}

void pimm::MoveForwardCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

void pimm::MoveForwardCommand::UndoCommand(AGameObject& gameObject)
{
	gameObject.GetTransform().SetPosition(m_previousPosition);
}

pimm::MoveForwardCommand::~MoveForwardCommand()
{
}
