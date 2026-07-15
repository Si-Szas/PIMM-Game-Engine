#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class ConstantBuffer final : public GraphicsResource
	{
		public:
			//COONSTRUCTOR
			ConstantBuffer(const ConstantBufferDescriptor& constantBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer{};
			ui32 m_size{};

		friend class DeviceContext;
	};
}
