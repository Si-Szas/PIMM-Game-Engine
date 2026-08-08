#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/Math/Matrix4x4.h>

namespace pimm
{
	class TransformComponent final : public AComponent
	{
		pimm_typeid(TransformComponent)

		public:
			//CONSTRUCTOR
			explicit TransformComponent(const AComponentDescriptor& descriptor);

			//GETTERS & SETTERS
			void SetPosition(const Vec3& position);
			Vec3 GetPosition() const noexcept;

			void SetScale(const Vec3& scale);
			Vec3 GetScale() const noexcept;

			void SetRotation(const Vec3& rotation);
			Vec3 GetRotation() const noexcept;

			Vec3 Forward();
			Vec3 Right();
			Vec3 Up();

			void UpdateWorldMatrix() noexcept;
			Matrix4x4 GetAffineWorldMatrix() noexcept;
			Matrix4x4 GetRigidWorldMatrix() noexcept;
			
			//We mark components as "dirty" when it needs to be updated
			void MarkAsDirty();
		private:
			Matrix4x4 GetLocalAffineMatrix() noexcept;
			Matrix4x4 GetLocalRigidMatrix() noexcept;

		private:
			Vec3 m_position{0.0f};
			Vec3 m_scale{1.0f};
			Vec3 m_rotation{0.0f};

			Matrix4x4 m_affineWorldMatrix{};
			Matrix4x4 m_rigidWorldMatrix{};


			bool m_isDirty{};
	};
}

