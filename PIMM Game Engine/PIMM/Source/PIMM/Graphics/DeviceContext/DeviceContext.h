#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <PIMM/Math/Vec4.h>

#include <span>

namespace pimm {

	class DeviceContext final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			explicit DeviceContext(const GraphicsResourceDescriptor& descriptor);
		
			//FUNCTION
			void ClearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color);
			void ExecuteCommandList(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext);

			//SETTER
			void SetGraphicsPipelineState(const GraphicsPipelineState& pipeline);
			void SetViewportSize(const Rect& size);

			void SetVertexBuffer(const VertexBuffer& buffer);
			void SetConstantBuffers(const std::span<ConstantBuffer*>& buffers);
			void SetIndexBuffer(const IndexBuffer& buffer);

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
			static constexpr std::size_t MaxConstantBuffersPerScene{ 16 };

		private:
			Microsoft::WRL::ComPtr <ID3D11DeviceContext> m_context{};
			ID3D11Buffer* m_constantBuffers[MaxConstantBuffersPerScene]{};
	
		friend class GraphicsDevice;
	};
}