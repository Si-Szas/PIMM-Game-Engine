#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <PIMM/Math/Vec4.h>

#include <span>
#include <array>

namespace pimm {

	class DeviceContext final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			explicit DeviceContext(const GraphicsResourceDescriptor& descriptor);
		
			//FUNCTION
			void ClearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color);
			void ClearAndSetFrameBuffer(const FrameBuffer& frameBuffer, const Vec4& color);
			void ExecuteCommandList(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext);

			//SETTER
			void SetGraphicsPipelineState(const GraphicsPipelineState& pipeline);
			void SetViewportSize(const Rect& size);

			void SetVertexBuffer(const VertexBuffer& buffer);
			void SetConstantBuffers(const std::span<ConstantBuffer*>& buffers);
			void SetIndexBuffer(const IndexBuffer& buffer);

			void SetTextures(const std::span<Texture*>& textures);
			void SetSamplers(const std::span<Sampler*>& samplers);

			//GETTER
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3D11DeviceContext();

			//UPDATE 
			void UpdateConstantBuffer(const ConstantBuffer& buffer, const std::span<const std::byte>& data);
			
			//DRAW
			void DrawTriangleList(ui32 vertexCount, ui32 startVertexLocation);
			void DrawTriangleListWithTessellation(ui32 vertexCount, ui32 startVertexLocation);
			void DrawQuadList(ui32 vertexCount, ui32 startVertexLocation);
			void Draw3PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation);
			void Draw4PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation);

		public:
			static constexpr std::size_t MaxConstantBuffersPerScene{ D3D11_COMMONSHADER_CONSTANT_BUFFER_API_SLOT_COUNT };
			static constexpr std::size_t MaxSamplersPerScene{ D3D11_COMMONSHADER_SAMPLER_SLOT_COUNT };
			static constexpr std::size_t MaxTexturesPerScene{ D3D11_COMMONSHADER_INPUT_RESOURCE_SLOT_COUNT };

		private:
			Microsoft::WRL::ComPtr <ID3D11DeviceContext> m_context{};

			std::array<ID3D11Buffer*, MaxConstantBuffersPerScene> m_constantBuffers{};
			std::array<ID3D11ShaderResourceView*, MaxTexturesPerScene> m_shaderResourceView{};
			std::array<ID3D11SamplerState*, MaxSamplersPerScene> m_samplers{};
	
		friend class GraphicsDevice;
	};
}