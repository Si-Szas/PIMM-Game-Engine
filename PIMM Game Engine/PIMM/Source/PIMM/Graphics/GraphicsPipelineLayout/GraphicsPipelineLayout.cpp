#include <PIMM/Graphics/GraphicsPipelineLayout/GraphicsPipelineLayout.h>
#include <PIMM/Graphics/ShaderBinary/ShaderBinary.h>
#include <PIMM/Graphics/GraphicsUtility/GraphicsUtility.h>
#include <d3dcompiler.h>
#include <ranges>

pimm::GraphicsPipelineLayout::GraphicsPipelineLayout(const GraphicsPipelineLayoutDescriptor& graphicsPipelineLayoutDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor), 
	m_vsBinary(graphicsPipelineLayoutDescriptor.vertexShaderBinary),
	m_hsBinary(graphicsPipelineLayoutDescriptor.hullShaderBinary),
	m_dsBinary(graphicsPipelineLayoutDescriptor.domainShaderBinary),
	m_psBinary(graphicsPipelineLayoutDescriptor.pixelShaderBinary)
{
	//CHECK VERTEX SHADER//
	if (!graphicsPipelineLayoutDescriptor.vertexShaderBinary) 
		PIMMLogThrowInvalidArgument("No shader binary provided.");
	if (graphicsPipelineLayoutDescriptor.vertexShaderBinary->GetShaderType() != ShaderType::VertexShader)
		PIMMLogThrowInvalidArgument("The 'vertexShaderBinary' member is not a valid vertex shader binary.");
	//CHECK HULL SHADER//
	if (!graphicsPipelineLayoutDescriptor.hullShaderBinary)
		PIMMLogThrowInvalidArgument("No shader binary provided.");
	if (graphicsPipelineLayoutDescriptor.hullShaderBinary->GetShaderType() != ShaderType::HullShader)
		PIMMLogThrowInvalidArgument("The 'hullShaderBinary' member is not a valid hull shader binary.");
	//CHECK DOMAIN SHADER//
	if (!graphicsPipelineLayoutDescriptor.domainShaderBinary)
		PIMMLogThrowInvalidArgument("No shader binary provided.");
	if (graphicsPipelineLayoutDescriptor.domainShaderBinary->GetShaderType() != ShaderType::DomainShader)
		PIMMLogThrowInvalidArgument("The 'domainShaderBinary' member is not a valid domain shader binary.");
	//CHECK PIXEL SHADER//
	if (!graphicsPipelineLayoutDescriptor.pixelShaderBinary) 
		PIMMLogThrowInvalidArgument("No shader binary provided.");
	if (graphicsPipelineLayoutDescriptor.pixelShaderBinary->GetShaderType() != ShaderType::PixelShader)
		PIMMLogThrowInvalidArgument("The 'pixelShaderBinary' member is not a valid pixel shader binary.");

	ProcessShaderBinary(*m_vsBinary);
	ProcessShaderBinary(*m_hsBinary);
	ProcessShaderBinary(*m_dsBinary);
	ProcessShaderBinary(*m_psBinary);
}

pimm::BinaryData pimm::GraphicsPipelineLayout::GetVSBinaryData() const noexcept
{
	return m_vsBinary->GetData();
}

pimm::BinaryData pimm::GraphicsPipelineLayout::GetHSBinaryData() const noexcept
{
	return m_hsBinary->GetData();
}

pimm::BinaryData pimm::GraphicsPipelineLayout::GetDSBinaryData() const noexcept
{
	return m_dsBinary->GetData();
}

pimm::BinaryData pimm::GraphicsPipelineLayout::GetPSBinaryData() const noexcept
{
	return m_psBinary->GetData();
}

pimm::BinaryData pimm::GraphicsPipelineLayout::GetInputElementsData() const noexcept
{
	return
	{
		m_elements,
		m_numberOfElements
	};
}

pimm::ui32 pimm::GraphicsPipelineLayout::GetMaxTextureSlots() const noexcept
{
	return m_maxTextureSlots;
}

pimm::ui32 pimm::GraphicsPipelineLayout::GetMaxSamplerSlots() const noexcept
{
	return m_maxSamplerSlots;
}

pimm::ui32 pimm::GraphicsPipelineLayout::GetMaxConstantBufferSlots() const noexcept
{
	return m_maxBufferSlots;
}

void pimm::GraphicsPipelineLayout::ProcessShaderBinary(ShaderBinary& binary)
{
	auto data = binary.GetData();
	auto& reflection = m_shaderReflections[static_cast<ui32>(binary.GetShaderType())];

	PIMMGraphicsLogThrowOnFail(D3DReflect(
		data.data,
		data.dataSize,
		IID_PPV_ARGS(&reflection)),
	"D3DReflect() failed.");

	D3D11_SHADER_DESC shaderDesc{};
	PIMMGraphicsLogThrowOnFail(reflection->GetDesc(&shaderDesc),
		"ID3D11ShaderReflection::GetDesc failed.");

	if (binary.GetShaderType() == ShaderType::VertexShader)
	{
		m_numberOfElements = shaderDesc.InputParameters;
		D3D11_SIGNATURE_PARAMETER_DESC params[D3D11_STANDARD_VERTEX_ELEMENT_COUNT]{};
		for (auto i : std::views::iota(0u, m_numberOfElements))
		{
			PIMMGraphicsLogThrowOnFail(reflection->GetInputParameterDesc(i, &params[i]),
				"ID3D11ShaderReflection::GetInputParameterDesc failed.");
		}
		for (auto i : std::views::iota(0u, m_numberOfElements))
		{
			auto param = params[i];
			m_elements[i] = {
				param.SemanticName,				//Semantic name
				param.SemanticIndex,			//Semantic index (if they share the same type)
				pimm::GraphicsUtility::GetDXGIFormatFromMask(param.ComponentType, param.Mask),
				0,								//Input Slot
				D3D11_APPEND_ALIGNED_ELEMENT,	//Allows us to let D3D compute the current offset automatically by appending each consecutive 
				D3D11_INPUT_PER_VERTEX_DATA,	//Input slot class attribute that specifies if a vertex attributes comes per vertex or per instance
				0								//Instance step date attribute required for instance rendering
			};
		}
	}

	{
		D3D11_SHADER_INPUT_BIND_DESC desc{};
		for (auto i : std::views::iota(0u, shaderDesc.BoundResources))
		{
			PIMMGraphicsLogThrowOnFail(reflection->GetResourceBindingDesc(i, &desc),
				"ID3D11ShaderReflection::GetInputParameterDesc failed.");
			if (desc.Type == D3D_SIT_CBUFFER)
				m_maxBufferSlots = std::max(m_maxBufferSlots, desc.BindPoint + 1);
			if (desc.Type == D3D_SIT_TEXTURE)
				m_maxTextureSlots = std::max(m_maxTextureSlots, desc.BindPoint + 1);
			if (desc.Type == D3D_SIT_SAMPLER)
				m_maxSamplerSlots = std::max(m_maxSamplerSlots, desc.BindPoint + 1);
		}
	}
}
