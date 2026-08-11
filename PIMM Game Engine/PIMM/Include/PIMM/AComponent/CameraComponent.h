#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/Math/Matrix4x4.h>
#include <PIMM/Math/Rect.h>

namespace pimm
{
	class CameraComponent final : public AComponent
	{
			pimm_typeid(CameraComponent)
		public:
			//CONSTRUCTOR
			explicit CameraComponent(const AComponentDescriptor& descriptor);

			//GETTERS & SETTERS
			Matrix4x4 GetViewMatrix() noexcept;
			Matrix4x4 GetProjectionMatrix() const noexcept;

			void SetFarPlane(f32 farPlane) noexcept;
			f32 GetFarPlane() const noexcept;

			void SetNearPlane(f32 nearPlane) noexcept;
			f32 GetNearPlane() const noexcept;

			void SetFieldOfView(f32 fieldOfView) noexcept;
			f32 GetFieldOfView() const noexcept;

			void SetViewportSize(const Rect& size) noexcept;
			Rect GetViewportSize() const noexcept;

		private:
			//FUNCTIONS
			void ComputeProjectionMatrix() noexcept;

		private:
			Matrix4x4 m_projection{};

			f32 m_nearPlane = 0.01f;
			f32 m_farPlane = 100.0f;
			f32 m_fieldOfView = 1.5f;

			Rect m_viewportSize{ 1, 1 };

			bool m_dirty{ true }; 

	};
}