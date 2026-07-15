#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>

namespace pimm
{
	class SwapChain final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			SwapChain(const SwapChainDescriptor& swapChainDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
		
			//FUNCTIONS
			// Enable vsync to match refresh rate of my laptops monitor (60Hz)
			void Present(bool vsync = false);

			//GETTER
			Rect GetSize() const noexcept;
			ID3D11RenderTargetView* GetRenderTargetView() const noexcept;
			ID3D11DepthStencilView* GetDepthStencilView() const noexcept;

	private:
			//FUNCTIONS
			void ReloadBuffers();

		private:
			Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain{};
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_renderTargetView{};
			Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_depthStencilView{};
			Rect m_size{};

			friend class DeviceContext;
	};
}