#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class Sampler final : public GraphicsResource
	{
	public:
		Sampler(const SamplerDescriptor& samplerDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> m_sampler{};
		friend class DeviceContext;
	};
}
