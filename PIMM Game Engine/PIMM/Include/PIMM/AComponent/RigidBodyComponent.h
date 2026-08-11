#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/Core/Base.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/Math/Vec3.h>

#include <reactphysics3d/reactphysics3d.h>
#include <vector>

namespace pimm
{
	enum class BodyType
	{
		Static = 0,
		Kinematic,
		Dynamic
	};

	enum class ColliderType
	{
		Box = 0,
		Sphere,
		Capsule
	};

	struct ColliderInfo
	{
		ColliderType type = ColliderType::Box;
		Vec3 halfExtents{ 0.5f };
		f32 radius = 0.5f;
		f32 height = 1.0f;
	};

	class RigidBodyComponent final : public AComponent
	{
		pimm_typeid(RigidBodyComponent)

	public:
		//CONSTRUCTOR
		explicit RigidBodyComponent(const AComponentDescriptor& descriptor);

		//BODY TYPE / MASS
		void SetBodyType(BodyType type) noexcept;
		BodyType GetBodyType() const noexcept;

		void SetMass(f32 mass) noexcept;
		f32 GetMass() const noexcept;

		void EnableGravity(bool enabled) noexcept;
		bool IsGravityEnabled() const noexcept;

		//COLLIDERS - halfExtents/radius are WORLD units, not affected by TransformComponent scale
		void AddBoxCollider(const Vec3& halfExtents);
		void AddSphereCollider(f32 radius);
		void AddCapsuleCollider(f32 radius, f32 height);
		void RemoveAllColliders();
		void RestoreLastCollider();

		ui32 GetColliderCount() const noexcept { return static_cast<ui32>(m_colliders.size()); }
		const ColliderInfo& GetCollider(ui32 index) const noexcept { return m_colliders[index]; }

		//FORCES
		void ApplyForce(const Vec3& worldForce);
		void ApplyTorque(const Vec3& worldTorque);

		void SetLinearVelocity(const Vec3& velocity);
		Vec3 GetLinearVelocity() const noexcept;

		void SetAngularVelocity(const Vec3& velocity);
		Vec3 GetAngularVelocity() const noexcept;

		//SYNC
		//Called automatically every frame by World after stepping the physics simulation
		void SyncTransformFromPhysics();
		//Call manually if you teleport the object via TransformComponent and want physics to match
		void SyncPhysicsFromTransform();

		rp3d::RigidBody* GetNativeBody() noexcept;

		//DESTRUCTOR
		~RigidBodyComponent();

	private:
		rp3d::RigidBody* m_rigidBody{};
		BodyType m_bodyType{ BodyType::Dynamic };
		bool m_savedGravityEnabled{ true };
		std::vector<ColliderInfo> m_colliders{};
		std::vector<rp3d::Collider*> m_colliderShapes{};
		ColliderInfo m_lastColliderInfo{};
		bool m_hasLastCollider{ false };
	};
}