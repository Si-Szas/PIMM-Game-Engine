#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/Core/Identifier.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/AGameObject/Cube.h>

#include <reactphysics3d/reactphysics3d.h>

#include <unordered_map>
#include <vector>
#include <span>

namespace pimm
{
	class World final : public Base
	{
	public:
		//CONSTRUCTOR
		explicit World(const WorldDescriptor& worldDescriptor);
		~World();

		//FUNCTIONS
		template <typename Type>
		Type* CreateAGameObject() requires IsRegistered<AGameObject, Type>
		{
			static_assert(std::is_base_of<AGameObject, Type>::value, "Type must inherit from pimm::AGameObject.");
			static_assert(HasTypeID<Type>, "Type needs a unique TypeID. Make sure you have added pimm_typeid and applied it to the correct class.");
			AGameObjectDescriptor descriptor{
				.base = {m_logger},
				.gameContext = m_gameContext,
				.world = *this,
				.worldRenderer = m_worldRenderer
			};
			std::unique_ptr<Type> typedPointer = std::make_unique<Type>(descriptor);
			UniquePtr<AGameObject> gameObjEvent = std::move(typedPointer);
			return static_cast<Type*>(CreateAGameObjectInternal(gameObjEvent));
		}

		template <typename Type>
		Type* const* GetAGameObject(ui32& numberOfComponents) const noexcept
		{
			return reinterpret_cast<Type* const*>(GetAGameObjectsInternal(Type::getTypeId(), &numberOfComponents));
		}

		template <typename Type>
		Type* const* GetAComponent(ui32& numberOfComponents) const noexcept
		{
			return reinterpret_cast<Type* const*>(CreateAComponentsInternal(Type::getTypeId(), &numberOfComponents));
		}

		AGameObject* CreateAGameObjectInternal(UniquePtr<AGameObject>& object);
		AComponent* const* CreateAComponentsInternal(size_t typeID, ui32* numberOfComponents) const noexcept;
		std::span<pimm::AGameObject* const> GetAllGameObjects() const noexcept;
		AGameObject* const* GetAGameObjectsInternal(size_t typeID, ui32* numberOfObjects) const noexcept;

		void AddComponentInternal(AComponent& component);
		void AddDirtyTransformInternal(TransformComponent& transformComponent);
		void DestroyAGameObject(AGameObject* object);
		void DeleteAllAGameObjects();
		void DestroyAGameObjectInternal(AGameObject* object);
		void DestroyAllAGameObjectsInternal();
		void Update(f32 deltaTime);

		//INSPECTOR PANEL
		void SetSelectedObjectIndex(ui32 newIndex);
		ui32 GetSelectedObjectIndex();

		AGameObject* GetSelectedGameObject();

		//PHYSICS
		rp3d::PhysicsWorld& GetPhysicsWorld() noexcept { return *m_physicsWorld; }
		rp3d::PhysicsCommon& GetPhysicsCommon() noexcept { return m_physicsCommon; }

		void SetPhysicsEnabled(bool enabled) noexcept;
		void ResetPhysicsAccumulator() noexcept;
		void StepPhysicsFrame();
		bool IsPhysicsEnabled() const noexcept { return m_physicsEnabled; }

	private:
		enum class EventType
		{
			Create = 0,
			Destroy,
			DestroyAll
		};

		struct AGameObjectEvent
		{
			AGameObject* object{};
			size_t pendingObjectIndex{};
			EventType eventType{};
		};

		std::vector<AGameObject*> m_allObjects{};
		std::unordered_map<size_t, std::vector<UniquePtr<AGameObject>>> m_objects{};
		std::unordered_map<size_t, std::vector<AComponent*>> m_components{};
		std::vector<TransformComponent*> m_dirtyTransforms{};
		std::vector<UniquePtr<AGameObject>> m_pendingObjects;
		std::vector<UniquePtr<AGameObject>> m_pendingObjectsSwapBuffer;
		std::vector<AGameObjectEvent> m_events{};
		std::vector<AGameObjectEvent> m_eventsSwapBuffer{};
		GameContext m_gameContext;
		WorldRenderer& m_worldRenderer;

		//INSPECTOR PANEL
		AGameObject* m_selectedGameObject{};
		ui32 m_selectedObjectIndex = 0;

		//PHYSICS
		rp3d::PhysicsCommon m_physicsCommon;
		rp3d::PhysicsWorld* m_physicsWorld{};
		f32 m_physicsAccumulator{};
		bool m_physicsEnabled = false;

		friend class AGameObject;
		friend class AComponent;
	};
}