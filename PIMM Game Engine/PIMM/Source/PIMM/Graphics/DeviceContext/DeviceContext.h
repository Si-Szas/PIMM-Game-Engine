#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <PIMM/Math/Vec4.h>

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
			void SetAllConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer);
			void SetVSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer);
			void SetHSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer);
			void SetDSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer);
			void SetPSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer);
			void SetIndexBuffer(const IndexBuffer& buffer);

			//GETTER
			Microsoft::WRL::ComPtr<ID3D11DeviceContext> GetD3D11DeviceContext();

			//UPDATE 
			void UpdateConstantBuffer(const ConstantBuffer& buffer, const void* data);
			
			//DRAW
			void DrawTriangleList(ui32 vertexCount, ui32 startVertexLocation);
			void DrawTriangleListWithTessellation(ui32 vertexCount, ui32 startVertexLocation);
			void DrawQuadList(ui32 vertexCount, ui32 startVertexLocation);
			void Draw3PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation);
			void Draw4PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation);

		private:
			Microsoft::WRL::ComPtr <ID3D11DeviceContext> m_context{};
	
		friend class GraphicsDevice;
	};
}