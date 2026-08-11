#include <PIMM/AComponent/RigidBodyComponent.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/Game/World.h>
#include <PIMM/Math/MathUtility.h>

#include <cmath>

using namespace pimm;

namespace
{
	rp3d::Vector3 ToRP3D(const Vec3& v) noexcept
	{
		return rp3d::Vector3(v.x, v.y, v.z);
	}

	Vec3 FromRP3D(const rp3d::Vector3& v) noexcept
	{
		return Vec3(v.x, v.y, v.z);
	}

	rp3d::Quaternion ToRP3DQuat(const Vec3& eulerDegrees) noexcept
	{
		f32 rx = MathUtility::ToRadians(eulerDegrees.x) * 0.5f;
		f32 ry = MathUtility::ToRadians(eulerDegrees.y) * 0.5f;
		f32 rz = MathUtility::ToRadians(eulerDegrees.z) * 0.5f;

		rp3d::Quaternion qx(std::sin(rx), 0.0f, 0.0f, std::cos(rx));
		rp3d::Quaternion qy(0.0f, std::sin(ry), 0.0f, std::cos(ry));
		rp3d::Quaternion qz(0.0f, 0.0f, std::sin(rz), std::cos(rz));

		return qx * qy * qz;
	}

	Vec3 FromRP3DQuat(const rp3d::Quaternion& q) noexcept
	{
		f32 x = q.x, y = q.y, z = q.z, w = q.w;

		f32 r00 = 1.0f - 2.0f * (y * y + z * z);
		f32 r01 = 2.0f * (x * y + w * z);
		f32 r12 = 2.0f * (y * z + w * x);
		f32 r22 = 1.0f - 2.0f * (x * x + y * y);

		f32 sinY = 2.0f * (w * y - x * z);
		sinY = sinY < -1.0f ? -1.0f : (sinY > 1.0f ? 1.0f : sinY);

		f32 rx = std::atan2(r12, r22);
		f32 ry = std::asin(sinY);
		f32 rz = std::atan2(r01, r00);

		const f32 toDeg = 180.0f / MathUtility::PI;
		return Vec3(rx * toDeg, ry * toDeg, rz * toDeg);
	}
}

pimm::RigidBodyComponent::RigidBodyComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
	auto& transform = m_object.GetTransform();

	rp3d::Transform startTransform(
		ToRP3D(transform.GetPosition()),
		ToRP3DQuat(transform.GetRotation())
	);

	m_rigidBody = m_world.GetPhysicsWorld().createRigidBody(startTransform);
}

pimm::RigidBodyComponent::~RigidBodyComponent()
{
	if (m_rigidBody)
	{
		m_world.GetPhysicsWorld().destroyRigidBody(m_rigidBody);
		m_rigidBody = nullptr;
	}
}

void pimm::RigidBodyComponent::SetBodyType(BodyType type) noexcept
{
	m_bodyType = type;
	switch (type)
	{
	case BodyType::Static: m_rigidBody->setType(rp3d::BodyType::STATIC);    break;
	case BodyType::Kinematic: m_rigidBody->setType(rp3d::BodyType::KINEMATIC); break;
	case BodyType::Dynamic: m_rigidBody->setType(rp3d::BodyType::DYNAMIC);   break;
	}
}

BodyType pimm::RigidBodyComponent::GetBodyType() const noexcept
{
	return m_bodyType;
}

void pimm::RigidBodyComponent::SetMass(f32 mass) noexcept
{
	m_rigidBody->setMass(mass);
}

f32 pimm::RigidBodyComponent::GetMass() const noexcept
{
	return m_rigidBody->getMass();
}

void pimm::RigidBodyComponent::EnableGravity(bool enabled) noexcept
{
	m_rigidBody->enableGravity(enabled);
}

void pimm::RigidBodyComponent::AddBoxCollider(const Vec3& halfExtents)
{
	auto* shape = m_world.GetPhysicsCommon().createBoxShape(ToRP3D(halfExtents));
	m_rigidBody->addCollider(shape, rp3d::Transform::identity());
	m_rigidBody->updateMassPropertiesFromColliders();
	m_colliders.push_back({ ColliderType::Box, halfExtents, 0.0f, 0.0f });
}

void pimm::RigidBodyComponent::AddSphereCollider(f32 radius)
{
	auto* shape = m_world.GetPhysicsCommon().createSphereShape(radius);
	m_rigidBody->addCollider(shape, rp3d::Transform::identity());
	m_rigidBody->updateMassPropertiesFromColliders();
	m_colliders.push_back({ ColliderType::Sphere, {}, radius, 0.0f });
}

void pimm::RigidBodyComponent::AddCapsuleCollider(f32 radius, f32 height)
{
	auto* shape = m_world.GetPhysicsCommon().createCapsuleShape(radius, height);
	m_rigidBody->addCollider(shape, rp3d::Transform::identity());
	m_rigidBody->updateMassPropertiesFromColliders();
	m_colliders.push_back({ ColliderType::Capsule, {}, radius, height });
}

void pimm::RigidBodyComponent::ApplyForce(const Vec3& worldForce)
{
	m_rigidBody->applyWorldForceAtCenterOfMass(ToRP3D(worldForce));
}

void pimm::RigidBodyComponent::ApplyTorque(const Vec3& worldTorque)
{
	m_rigidBody->applyWorldTorque(ToRP3D(worldTorque));
}

void pimm::RigidBodyComponent::SetLinearVelocity(const Vec3& velocity)
{
	m_rigidBody->setLinearVelocity(ToRP3D(velocity));
}

Vec3 pimm::RigidBodyComponent::GetLinearVelocity() const noexcept
{
	return FromRP3D(m_rigidBody->getLinearVelocity());
}

void pimm::RigidBodyComponent::SetAngularVelocity(const Vec3& velocity)
{
	m_rigidBody->setAngularVelocity(ToRP3D(velocity));
}

Vec3 pimm::RigidBodyComponent::GetAngularVelocity() const noexcept
{
	return FromRP3D(m_rigidBody->getAngularVelocity());
}

void pimm::RigidBodyComponent::SyncTransformFromPhysics()
{
	if (m_bodyType == BodyType::Kinematic || m_bodyType == BodyType::Static) return;

	const rp3d::Transform& rpTransform = m_rigidBody->getTransform();
	auto& transform = m_object.GetTransform();

	transform.SetPosition(FromRP3D(rpTransform.getPosition()));
	transform.SetRotation(FromRP3DQuat(rpTransform.getOrientation()));
}

void pimm::RigidBodyComponent::SyncPhysicsFromTransform()
{
	auto& transform = m_object.GetTransform();

	rp3d::Transform rpTransform(
		ToRP3D(transform.GetPosition()),
		ToRP3DQuat(transform.GetRotation())
	);

	m_rigidBody->setTransform(rpTransform);
}

rp3d::RigidBody* pimm::RigidBodyComponent::GetNativeBody() noexcept
{
	return m_rigidBody;
}