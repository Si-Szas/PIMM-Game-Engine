#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <d3dcompiler.h>

namespace pimm
{
	class GraphicsPipelineLayout final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			GraphicsPipelineLayout(const GraphicsPipelineLayoutDescriptor& graphicsPipelineLayoutDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
			//GETTER
			BinaryData GetVSBinaryData() const noexcept;
			BinaryData GetHSBinaryData() const noexcept;
			BinaryData GetDSBinaryData() const noexcept;
			BinaryData GetPSBinaryData() const noexcept;
			BinaryData GetInputElementsData() const noexcept;
			ui32 GetMaxTextureSlots() const noexcept;
			ui32 GetMaxSamplerSlots() const noexcept;
			ui32 GetMaxConstantBufferSlots() const noexcept;

		private:
			void ProcessShaderBinary(ShaderBinary& binary);

		private:
			RefPtr<ShaderBinary> m_vsBinary{};
			RefPtr<ShaderBinary> m_hsBinary{};
			RefPtr<ShaderBinary> m_dsBinary{};
			RefPtr<ShaderBinary> m_psBinary{};

			Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_shaderReflections[4]{};
			
			D3D11_INPUT_ELEMENT_DESC m_elements[D3D11_STANDARD_VERTEX_ELEMENT_COUNT]{};
			
			ui32 m_numberOfElements{};
			ui32 m_maxTextureSlots{};
			ui32 m_maxSamplerSlots{};
			ui32 m_maxBufferSlots{};
	};
}

