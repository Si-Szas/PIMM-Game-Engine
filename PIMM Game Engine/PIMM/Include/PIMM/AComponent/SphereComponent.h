#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class SphereComponent final : public AComponent
	{
		pimm_typeid(SphereComponent)
		public:
			//CONSTRUCTOR
			explicit SphereComponent(const AComponentDescriptor& descriptor);

			//GETTERS
			VertexBuffer& GetVertexBuffer();
			IndexBuffer& GetIndexBuffer();
			f32 GetRadius();

			//SETTERS
			//void SetRadius(f32 newRadius);

		private:
			RefPtr<VertexBuffer> m_vertexBuffer{};
			RefPtr<IndexBuffer> m_indexBuffer{};

			f32 m_radius = 0.5f;
			ui32 m_sliceCount = 50 * m_radius;
			ui32 m_stackCount = 50 * m_radius;
	};
}

