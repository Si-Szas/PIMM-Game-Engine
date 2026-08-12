#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/RigidBodyComponent.h>
#include <PIMM/AGameObject/CameraObject.h>

#include <algorithm>
#include <span>

pimm::World::World(const WorldDescriptor& descriptor) : Base(descriptor.base),
m_gameContext(descriptor.gameContext),
m_worldRenderer(descriptor.worldRenderer)
{
	m_physicsWorld = m_physicsCommon.createPhysicsWorld();
}

pimm::World::~World()
{
	DeleteAllAGameObjects();

	if (m_physicsWorld)
	{
		m_physicsCommon.destroyPhysicsWorld(m_physicsWorld);
		m_physicsWorld = nullptr;
	}
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
			if (object->IsEnabled())
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

	if (m_physicsEnabled)
	{
		constexpr f32 physicsTimeStep = 1.0f / 60.0f;
		m_physicsAccumulator += deltaTime;
		while (m_physicsAccumulator >= physicsTimeStep)
		{
			m_physicsWorld->update(physicsTimeStep);
			m_physicsAccumulator -= physicsTimeStep;
		}

		auto rigidBodyIt = m_components.find(RigidBodyComponent::getTypeId());
		if (rigidBodyIt != m_components.end())
		{
			for (AComponent* component : rigidBodyIt->second)
			{
				static_cast<RigidBodyComponent*>(component)->SyncTransformFromPhysics();
			}
		}
	}
}

void pimm::World::SetPhysicsEnabled(bool enabled) noexcept
{
	m_physicsEnabled = enabled;
}

void pimm::World::ResetPhysicsAccumulator() noexcept
{
	m_physicsAccumulator = 0.0f;
}

void pimm::World::StepPhysicsFrame()
{
	constexpr f32 physicsTimeStep = 1.0f / 60.0f;
	m_physicsWorld->update(physicsTimeStep);

	auto rigidBodyIt = m_components.find(RigidBodyComponent::getTypeId());
	if (rigidBodyIt != m_components.end())
	{
		for (AComponent* component : rigidBodyIt->second)
		{
			static_cast<RigidBodyComponent*>(component)->SyncTransformFromPhysics();
		}
	}
}

void pimm::World::SetSelectedObjectIndex(ui32 newIndex)
{
	auto gameObjects = GetAllGameObjects();

	if (newIndex >= gameObjects.size())
	{
		m_selectedObjectIndex = 0;
		m_selectedObjects.clear();
		return;
	}

	m_selectedObjectIndex = newIndex;
	m_selectedObjects.clear();
	m_selectedObjects.push_back(gameObjects[newIndex]);
}

pimm::ui32 pimm::World::GetSelectedObjectIndex()
{
	return m_selectedObjectIndex;
}

pimm::AGameObject* pimm::World::GetSelectedGameObject()
{
	if (m_selectedObjects.empty())
		return nullptr;
	return m_selectedObjects[0];
}

std::span<pimm::AGameObject* const> pimm::World::GetSelectedGameObjects() const noexcept
{
	return m_selectedObjects;
}

bool pimm::World::IsSelected(const AGameObject* object) const noexcept
{
	return std::find(m_selectedObjects.begin(), m_selectedObjects.end(), object) != m_selectedObjects.end();
}

void pimm::World::SelectObject(AGameObject* object)
{
	if (!object)
		return;
	if (!IsSelected(object))
		m_selectedObjects.push_back(object);
}

void pimm::World::DeselectObject(AGameObject* object)
{
	if (!object)
		return;
	auto it = std::find(m_selectedObjects.begin(), m_selectedObjects.end(), object);
	if (it != m_selectedObjects.end())
		m_selectedObjects.erase(it);
}

void pimm::World::ToggleSelection(AGameObject* object)
{
	if (!object)
		return;
	if (IsSelected(object))
		DeselectObject(object);
	else
		SelectObject(object);
}

void pimm::World::DeselectAllObjects()
{
	m_selectedObjects.clear();
}

void pimm::World::SetSearchFilter(const std::string& filter) noexcept
{
	m_searchFilter = filter;
}

const std::string& pimm::World::GetSearchFilter() const noexcept
{
	return m_searchFilter;
}

pimm::CameraObject* pimm::World::GetActiveCameraObject() const noexcept
{
	for (auto* object : m_allObjects)
	{
		if (!object || object->GetTypeID() != CameraObject::getTypeId()) continue;

		auto* camera = static_cast<CameraObject*>(object);
		if (auto* component = camera->GetComponent<CameraComponent>(); component && component->IsActive())
			return camera;
	}

	for (auto* object : m_allObjects)
	{
		if (object && object->GetTypeID() == CameraObject::getTypeId())
			return static_cast<CameraObject*>(object);
	}

	return nullptr;
}

bool pimm::World::HasActiveCameraObject() const noexcept
{
	for (auto* object : m_allObjects)
	{
		if (!object || object->GetTypeID() != CameraObject::getTypeId()) continue;

		auto* camera = static_cast<CameraObject*>(object);
		if (auto* component = camera->GetComponent<CameraComponent>(); component && component->IsActive())
			return true;
	}

	return false;
}

void pimm::World::SetActiveCameraObject(CameraObject* camera) noexcept
{
	for (auto* object : m_allObjects)
	{
		if (!object || object->GetTypeID() != CameraObject::getTypeId()) continue;

		auto* cameraObject = static_cast<CameraObject*>(object);
		if (auto* component = cameraObject->GetComponent<CameraComponent>())
			component->SetActive(cameraObject == camera);
	}
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

void pimm::World::RemoveComponentInternal(AComponent& component)
{
	auto typeID = component.GetTypeID();
	auto it = m_components.find(typeID);
	if (it != m_components.end())
	{
		auto& componentList = it->second;
		auto componentIt = std::find(componentList.begin(), componentList.end(), &component);
		if (componentIt != componentList.end())
		{
			componentList.erase(componentIt);
		}
	}
}

void pimm::World::AddDirtyTransformInternal(TransformComponent& transformComponent)
{
	m_dirtyTransforms.push_back(&transformComponent);
}

void pimm::World::DestroyAGameObject(AGameObject* object)
{
	//Make sure the object exists
	if (!object) return;

	//Remove the pointer tracking the object in the allObjects span
	auto rawPointer = std::find(m_allObjects.begin(), m_allObjects.end(), object);
	if (rawPointer != m_allObjects.end()) m_allObjects.erase(rawPointer);

	//Remove this object's components from the World's component registry.
	//m_components stores raw pointers, so without this they'd dangle once the
	//unique_ptr<AComponent> entries inside the object are destroyed below.
	for (auto& [typeID, component] : object->m_components)
	{
		auto& componentList = m_components[typeID];
		auto componentIt = std::find(componentList.begin(), componentList.end(), component.get());
		if (componentIt != componentList.end())
		{
			componentList.erase(componentIt);
		}
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
	m_allObjects.clear();
	m_dirtyTransforms.clear();

	m_worldRenderer.GetVertexBuffer().clear();
	m_worldRenderer.GetIndexBuffer().clear();

	m_components.clear();

	m_pendingObjects.clear();
	m_pendingObjectsSwapBuffer.clear();

	m_events.clear();
	m_eventsSwapBuffer.clear();

	m_objects.clear();

	m_selectedObjects.clear();
	m_selectedObjectIndex = 0;

	m_searchFilter.clear();
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