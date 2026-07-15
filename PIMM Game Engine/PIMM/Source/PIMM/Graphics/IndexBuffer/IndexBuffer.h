#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class IndexBuffer final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			IndexBuffer(const IndexBufferDescriptor& indexBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
			
			//GETTER
			ui32 GetIndexListSize() const noexcept;

		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer{};
			ui32 m_size{};

		friend class DeviceContext;
	};
}
