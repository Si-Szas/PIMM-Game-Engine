#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class Texture final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			Texture(const TextureDescriptor& textureDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);

		private:
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_texture{};
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_shaderResourceView{};
			
		friend class DeviceContext;
	};
}
