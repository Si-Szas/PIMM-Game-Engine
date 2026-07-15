#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <PIMM/Graphics/GraphicsUtility/GraphicsLogUtility.h>

namespace pimm
{
	//Refers to collection of configuration settings and objects that control how the GPU process and renders graphics
		//How shaders are used, how pixels are blended, etc.
	class GraphicsPipelineState final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			GraphicsPipelineState(const GraphicsPipelineStateDescriptor& graphicsPipelineStateDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
		private:
			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader{};
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader{};
			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout{};
			Microsoft::WRL::ComPtr<ID3D11HullShader> m_hullShader{};
			Microsoft::WRL::ComPtr<ID3D11DomainShader> m_domainShader{};

		friend class DeviceContext;
	};
}

