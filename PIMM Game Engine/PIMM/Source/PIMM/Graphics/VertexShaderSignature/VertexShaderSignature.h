#pragma once
#include <PIMM/Graphics/GraphicsResource/GraphicsResource.h>
#include <d3dcompiler.h>

namespace pimm
{
	class VertexShaderSignature final : public GraphicsResource
	{
		public:
			//CONSTRUCTOR
			VertexShaderSignature(const VertexShaderSignatureDescriptor& vertexShaderSignatureDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor);
	
			//GETTER
			BinaryData GetShaderBinaryData() const noexcept;
			BinaryData GetInputElementsData() const noexcept;

		private:
			RefPtr<ShaderBinary> m_vertexShaderBinary{};
			Microsoft::WRL::ComPtr<ID3D11ShaderReflection> m_shaderReflection{};
			D3D11_INPUT_ELEMENT_DESC m_elements[D3D11_STANDARD_VERTEX_ELEMENT_COUNT]{};
			ui32 m_numberOfElements{};
	};
}

