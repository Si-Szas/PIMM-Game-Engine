#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>

#include <span>

pimm::World::World(const WorldDescriptor& descriptor) : Base(descriptor.base),
	m_gameContext(descriptor.gameContext),
	m_worldRenderer(descriptor.worldRenderer)
{

}

void pimm::World::Update(f32 deltaTime)
{
	if (m_events.size())
	{
		std::swap(m_events, m_eventsSwapBuffer);
		std::swap(m_pendingObjects, m_pendingObjectsSwapBuffer);
		
		for (auto& gameObjEvent : m_eventsSwapBuffer)
		{
			//Delete all game objects
			if (gameObjEvent.eventType == EventType::DestroyAll)
			{
				this->DeleteAllAGameObjects();
				continue;
			}

			//Prevent null error if all objects are gone
			if (!gameObjEvent.object) continue;

			auto objectTypeID = gameObjEvent.object->GetTypeID();
			auto pendingObjectIndex = gameObjEvent.pendingObjectIndex;
			//If object is to be created, creaete object
			if (gameObjEvent.eventType == EventType::Create)
			{
				auto& obj = m_pendingObjectsSwapBuffer[pendingObjectIndex];
				auto ptr = obj.get();

				m_objects[objectTypeID].push_back(std::move(obj));

				ptr->OnCreate();
			}
			//Destroy an Object
			if (gameObjEvent.eventType == EventType::Destroy)
			{
				this->DestroyAGameObject(gameObjEvent.object);
			}
		}

		m_pendingObjectsSwapBuffer.clear();
		m_eventsSwapBuffer.clear();
	}

	for (auto&& [typeID, objects] : m_objects)
	{
		for (auto& object : objects)
		{
			object->OnUpdate(deltaTime);
		}
	}

	//Update the transform components if they were marked as "dirty"
	for (auto& component : m_dirtyTransforms)
	{
		component->UpdateWorldMatrix();
	}

	//Clear the list of dirty component to ensure that they don't get included in next update
	m_dirtyTransforms.clear();

}

pimm::AGameObject* pimm::World::CreateAGameObjectInternal(UniquePtr<pimm::AGameObject>& object)
{
	if (object) 
	{

		auto pointer = object.get();

		size_t typeID = pointer->getTypeId();

		m_allObjects.push_back(pointer);

		auto index = m_pendingObjects.size();

		m_pendingObjects.push_back(std::move(object));
		m_events.push_back({ pointer, index, EventType::Create });

		return pointer;
	}

	return {};
}

pimm::AComponent* const* pimm::World::CreateAComponentsInternal(size_t typeID, ui32* numberOfComponents) const noexcept
{
	auto it = m_components.find(typeID);

	if (it != m_components.end())
	{
		*numberOfComponents = static_cast<ui32>(it->second.size());
		return it->second.data();
	}

	*numberOfComponents = 0u;
	return {};
}

pimm::AGameObject* const* pimm::World::GetAGameObjectsInternal(size_t typeID, ui32* numberOfObjects) const noexcept
{
	auto obj = m_objects.find(typeID);

	if (obj == m_objects.end() || obj->second.empty())
	{
		if (numberOfObjects) *numberOfObjects = 0;
		return nullptr;
	}

	const auto& uniquePtrs = obj->second;
	if (numberOfObjects)
	{
		*numberOfObjects = static_cast<ui32>(uniquePtrs.size());
	}

	return reinterpret_cast<AGameObject* const*>(uniquePtrs.data());
}

void pimm::World::AddComponentInternal(AComponent& component)
{
	//Add a component via ID
	auto typeID = component.GetTypeID();
	m_components[typeID].push_back(&component);
}

void pimm::World::AddDirtyTransformInternal(TransformComponent& transformComponent)
{
	m_dirtyTransforms.push_back(&transformComponent);
}

void pimm::World::DestroyAGameObject(AGameObject* object)
{
	//Make sure the object exists
	if (!object) return;

	//Get vertex offset and index location of object to delete
	ui32 vertexOffset = object->GetVertexOffset();
	ui32 indexLocation = object->GetIndexLocation();

	//Remove the pointer tracking the object in the allObjects span
	auto rawPointer = std::find(m_allObjects.begin(), m_allObjects.end(), object);
	if (rawPointer != m_allObjects.end()) m_allObjects.erase(rawPointer);

	//Remove the vertex and index buffer of the object to be deleted
	auto& vertexBuffer = m_worldRenderer.GetVertexBuffer();
	auto& indexBuffer = m_worldRenderer.GetIndexBuffer();
	//If statement to make sure that the offset isnt over the size of the buffer vector
	//Delete the buffer of the object
	if (vertexOffset < vertexBuffer.size()) vertexBuffer.erase(vertexBuffer.begin() + vertexOffset);
	if (indexLocation < indexBuffer.size()) indexBuffer.erase(indexBuffer.begin() + indexLocation);

	//Update the vertex offsets and index locations of the objects remaining in the vector
	//All objects just move by -1
	for (AGameObject* remainingObjects : m_allObjects)
	{
		//If the offset is greater than the offset of the object deleted (that means it is after), then move it back 
		if (remainingObjects->GetVertexOffset() > vertexOffset) remainingObjects->SetVertexOffset(remainingObjects->GetVertexOffset() - 1);
		//Same for index buffer
		if (remainingObjects->GetIndexLocation() > indexLocation) remainingObjects->SetIndexLocation(remainingObjects->GetIndexLocation() - 1);
	}

	//Get the type ID of the object to be deleted, and find it within m_objects
	size_t objectTypeID = object->GetTypeID();
	auto objectMap = m_objects.find(objectTypeID);
	//Then, delete the object 
	if (objectMap != m_objects.end())
	{
		auto& typeVector = objectMap->second;
		auto objectUniquePointer = std::find_if(typeVector.begin(), typeVector.end(),
			[object](const UniquePtr<AGameObject>& ptr) {
				return ptr.get() == object;
			});

		if (objectUniquePointer != typeVector.end())
		{
			typeVector.erase(objectUniquePointer);
		}
	}
}

void pimm::World::DeleteAllAGameObjects()
{
	// DELETE ALL GAME OBJECTS
	//  - Clear the pointers from m_allObjects
	//  - Clear all dirty transforms (makes sure nothing gets updated)
	//  - Clear Vertex and Index buffer from world renderer
	//  - Clear components and objects

	m_allObjects.clear();
	m_dirtyTransforms.clear();

	m_worldRenderer.GetVertexBuffer().clear();
	m_worldRenderer.GetIndexBuffer().clear();

	m_components.clear();
	m_objects.clear();
}

void pimm::World::DestroyAGameObjectInternal(AGameObject* object)
{
	if (!object) return;
	m_events.push_back(AGameObjectEvent{
		.object = object, //Object we want to delete
		.eventType = EventType::Destroy
	});
}

void pimm::World::DestroyAllAGameObjectsInternal()
{
	m_events.push_back(AGameObjectEvent{
		.object = nullptr, //Null since we're deleting all objects
		.eventType = EventType::DestroyAll
		});
}

std::span<pimm::AGameObject* const> pimm::World::GetAllGameObjects() const noexcept
{
	return m_allObjects;
}