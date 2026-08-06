#pragma once
#include <PIMM/Core/Common.h>
#include <PIMM/AComponent/AComponent.h>

namespace pimm
{
	class CubeComponent final : public AComponent
	{
		pimm_typeid(CubeComponent)
		public:
			//CONSTRUCTOR
			explicit CubeComponent(const AComponentDescriptor& descriptor);

			//GETTERS
			VertexBuffer& GetVertexBuffer();
			IndexBuffer& GetIndexBuffer();

		private:
			RefPtr<VertexBuffer> m_vertexBuffer{};
			RefPtr<IndexBuffer> m_indexBuffer{};
	};
}

