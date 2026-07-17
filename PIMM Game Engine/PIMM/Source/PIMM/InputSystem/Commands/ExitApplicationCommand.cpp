#include <PIMM/InputSystem/Commands/ExitApplicationCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <Windows.h>

pimm::ExitApplicationCommand::ExitApplicationCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::ExitApplicationCommand::ExecuteCommand()
{
	PostQuitMessage(0);
}

void pimm::ExitApplicationCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	PostQuitMessage(0);
}

void pimm::ExitApplicationCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

void pimm::ExitApplicationCommand::UndoCommand(AGameObject& gameObject)
{

}

pimm::ExitApplicationCommand::~ExitApplicationCommand()
{
}
