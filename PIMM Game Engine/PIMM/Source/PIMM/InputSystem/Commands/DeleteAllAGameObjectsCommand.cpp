#include <PIMM/InputSystem/Commands/DeleteAllAGameObjectsCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Game/World.h>

pimm::DeleteAllAGameObjectsCommand::DeleteAllAGameObjectsCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::DeleteAllAGameObjectsCommand::ExecuteCommand()
{
	//auto gameObjects = m_world->GetAllGameObjects();
	//
	//if (!gameObjects.empty()) m_world->DestroyAllAGameObjectsInternal();
}

void pimm::DeleteAllAGameObjectsCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	auto gameObjects = world.GetAllGameObjects();
	
	if (!gameObjects.empty()) world.DestroyAllAGameObjectsInternal();
}

void pimm::DeleteAllAGameObjectsCommand::UndoCommand(AGameObject& gameObject)
{

}

pimm::DeleteAllAGameObjectsCommand::~DeleteAllAGameObjectsCommand()
{

}
