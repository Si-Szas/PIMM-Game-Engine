#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class CylinderComponent final : public AComponent
	{
		pimm_typeid(CylinderComponent)
		public:
			//CONSTRUCTOR
			explicit CylinderComponent(const AComponentDescriptor& descriptor); 

			//GETTERS
			VertexBuffer& GetVertexBuffer();
			IndexBuffer& GetIndexBuffer();

		private:
			RefPtr<VertexBuffer> m_vertexBuffer{};
			RefPtr<IndexBuffer> m_indexBuffer{};

			//Defines how smooth the cylinder looks
			f32 m_height = 1.0f;
			f32 m_radius = 0.5f;
			ui32 m_sliceCount = 50 * m_radius;
			ui32 m_stackCount = 10 * m_height;
	};
}