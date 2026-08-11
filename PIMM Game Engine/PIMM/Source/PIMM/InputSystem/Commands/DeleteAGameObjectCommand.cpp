#include <PIMM/InputSystem/Commands/DeleteAGameObjectCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Game/World.h>

pimm::DeleteAGameObjectCommand::DeleteAGameObjectCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::DeleteAGameObjectCommand::ExecuteCommand()
{

}

void pimm::DeleteAGameObjectCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	auto gameObjects = world.GetAllGameObjects();
	//If the list of objects are not empty, then delete, and if its not the player (since player at index 0)
	//Player: Index 0, Floor: Index 1
	if (static_cast<ui32>(gameObjects.size()) > 1) world.DestroyAGameObjectInternal(gameObjects.back());
}

void pimm::DeleteAGameObjectCommand::ExecuteCommand(AGameObject& gameObject, World& world, ResourceManager& resourceManager)
{

}

void pimm::DeleteAGameObjectCommand::UndoCommand(AGameObject& gameObject)
{

}

pimm::DeleteAGameObjectCommand::~DeleteAGameObjectCommand()
{

}
