#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>


namespace pimm
{
	class VertexBuffer final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			VertexBuffer(const VertexBufferDescriptor& vertexBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
			//GETTER
			ui32 GetVertexListSize() const noexcept;

		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer{};
			ui32 m_vertexSize{};
			ui32 m_vertexListSize{};
	
		friend class DeviceContext;
	};
}
