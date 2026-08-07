#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Identifier.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/Math/Vec3.h>

#include <unordered_map>

namespace pimm
{
	class AGameObject : public Identifier
	{
		pimm_typeid(AGameObject)

	public:
		//CONSTRUCTOR
		explicit AGameObject(const AGameObjectDescriptor& descriptor);

		template <typename Type>
		Type* CreateOrGetComponent() requires IsRegistered<AComponent, Type>
		{
			auto comp = GetComponent<Type>();
			if (comp) return comp;
			UniquePtr<AComponent> component = std::make_unique<Type>(AComponentDescriptor{
				{m_logger},
				*this,
				m_world,
				m_gameContext
				});
			return static_cast<Type*>(CreateComponentInternal(component));
		}

		template <typename Type>
		Type* GetComponent() requires IsRegistered<AComponent, Type>
		{
			return static_cast<Type*>(GetComponentInternal(Type::getTypeId()));
		}

		//Get offset
		ui32 GetVertexOffset() noexcept;
		void SetVertexOffset(ui32 newOffset) noexcept;

		ui32 GetIndexLocation() noexcept;
		void SetIndexLocation(ui32 newLocation) noexcept;

		//Get the transform component
		TransformComponent& GetTransform() noexcept;
		World& GetWorld() noexcept;
		InputSystem& GetInputSystem() noexcept;
		const WorldRenderer& GetWorldRenderer() noexcept;
		ResourceManager& GetResourceManager() noexcept;

		//Get the material component
		MaterialComponent& GetMaterialComponent() noexcept;

		//Movement Modifiers
		void ResetMovementModifiers();

		Vec3 GetVelocity() const noexcept;
		void SetVelocity(Vec3 newVelocity) noexcept;

		f32 GetForwardModifier() const noexcept;
		void SetForwardModifier(f32 newForward) noexcept;

		f32 GetRightModifier() const noexcept;
		void SetRightModifier(f32 newRight) noexcept;

		f32 GetSpeedModifier() const noexcept;
		void SetSpeedModifier(f32 newSpeed) noexcept;

		//DESTRUCTOR
		virtual ~AGameObject();

	protected:
		virtual void OnCreate()
		{
		}
		virtual void OnUpdate(f32 deltaTime)
		{
		}

	private:
		AComponent* CreateComponentInternal(UniquePtr<AComponent>& component);
		AComponent* GetComponentInternal(size_t ID);

	private:
		std::unordered_map<size_t, UniquePtr<AComponent>> m_components{};

		// COMPONENTS SHARED BY ALL GAME OBJECTS
		TransformComponent* m_transform{};
		MaterialComponent* m_material{};

		GameContext m_gameContext;
		World& m_world;
		const WorldRenderer& m_worldRenderer;

	protected:
		// COUNTERS
		ui32 m_vertexOffset = 0;
		ui32 m_indexLocation = 0;

		// VELOCITY
		Vec3 m_velocity{ 0.0f };

		// MODIFIERS FOR MOVEMENT
		f32 forwardModifier, rightModifier = 0.0f;
		f32 speedModifier = 3.0f;

		friend class World;
	};
}