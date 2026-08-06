#pragma once
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/Core/Common.h>

namespace pimm {
	class CapsuleComponent final : public AComponent
	{
		pimm_typeid(CapsuleComponent)
		public:
			//CONSTRUCTOR
			explicit CapsuleComponent(const AComponentDescriptor& descriptor);

			//GETTERS
			VertexBuffer& GetVertexBuffer();
			IndexBuffer& GetIndexBuffer();
			f32 GetHeight();
			f32 GetRadius();

		private:
			RefPtr<VertexBuffer> m_vertexBuffer{};
			RefPtr<IndexBuffer> m_indexBuffer{};

			f32 m_height = 1.0f;
			f32 m_radius = 0.5f;
			ui32 m_sliceCount = 50 * m_radius;
			ui32 m_stackCount = 10 * m_height;
	};
}
