#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <PIMM/Math/Rect.h>

namespace pimm
{
	class FrameBuffer final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			FrameBuffer(const FrameBufferDescriptor& frameBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
			//FUNCTIONS
			void Create(GraphicsDevice& graphicsDevice, const FrameBufferDescriptor& frameBufferDescriptor);
			void Resize(GraphicsDevice& graphicsDevice, Rect newSize);

			ID3D11ShaderResourceView* GetSRV() const noexcept;
			ID3D11RenderTargetView* GetRTV() const noexcept;
			ID3D11DepthStencilView* GetDSV() const noexcept;
			ui32 GetFrameBufferSizeWidth() const noexcept;
			ui32 GetFrameBufferSizeHeight() const noexcept;

        private:
			//TO CHANGE EVENTUALLY TO UTILIZE THE CREATED TEXTURE CLASS!
            Microsoft::WRL::ComPtr<ID3D11Texture2D> m_colorTexture;
            Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_rtv;
            Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
            Microsoft::WRL::ComPtr<ID3D11DepthStencilView> m_dsv;


			// keep alive for one extra frame for resizing
			Microsoft::WRL::ComPtr<ID3D11Texture2D> m_previousColorTexture;
			Microsoft::WRL::ComPtr<ID3D11RenderTargetView> m_previousRtv;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_previousSrv;

			Rect m_size{ 0, 0 };
            ui32 m_sampleCount = 1;
	};
}