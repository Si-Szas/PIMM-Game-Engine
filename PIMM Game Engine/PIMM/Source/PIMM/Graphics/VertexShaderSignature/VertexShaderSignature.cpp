#include <PIMM/Graphics/VertexShaderSignature/VertexShaderSignature.h>
#include <PIMM/Graphics/ShaderBinary/ShaderBinary.h>
#include <PIMM/Graphics/GraphicsUtility/GraphicsUtility.h>
#include <d3dcompiler.h>
#include <ranges>

pimm::VertexShaderSignature::VertexShaderSignature(const VertexShaderSignatureDescriptor& vertexShaderSignatureDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor), m_vertexShaderBinary(vertexShaderSignatureDescriptor.vertexShaderBinary)
{

	if (!vertexShaderSignatureDescriptor.vertexShaderBinary) 
		PIMMLogThrowError("No Shader Binary provided.")
	if (vertexShaderSignatureDescriptor.vertexShaderBinary->GetShaderType() != ShaderType::VertexShader)
		PIMMLogThrowInvalidArgument("The 'vertexShaderBinary' member is not a valid vertex shader binary.");
	
	auto vertexShaderData = m_vertexShaderBinary->GetData();

	//Extract MetaData from compiled HLSL Shader
	//Enables inspection of various shader components
	PIMMGraphicsLogThrowOnFail(D3DReflect
	(
		vertexShaderData.data,				//Void pointer to shader binary data
		vertexShaderData.dataSize,			//Size in bytes of that data
		IID_PPV_ARGS(&m_shaderReflection)	//Output. Interface to reflect shader information
	), "D3DReflect() failed.");

	D3D11_SHADER_DESC shaderDescriptor{};
	//Retrieve shader's description
	PIMMGraphicsLogThrowOnFail(
		m_shaderReflection->GetDesc(&shaderDescriptor),
		"ID3D11ShaderReflection::GetDesc() failed."
	);

	m_numberOfElements = shaderDescriptor.InputParameters;

	//We need to define an array of signature parameter descriptor objects
	//D3D11 supports max 32 elements for vertex shader
	D3D11_SIGNATURE_PARAMETER_DESC params[D3D11_STANDARD_VERTEX_ELEMENT_COUNT]{};
	//Loop overall input parameters
	for(auto i : std::views::iota(0u, m_numberOfElements)) 
	{
		//Returns information about each input parameter in vertex shader
		PIMMGraphicsLogThrowOnFail(
		m_shaderReflection->GetInputParameterDesc(
			i,		   //Index of input parameter
			&params[i] //Where to store
		), "ID3D11ShaderReflection::GetInputParameterDesc() failed.");
	}

	/*
		constexpr D3D11_INPUT_ELEMENT_DESC elements[] =
	{	
		//// POSITION ATTRIBUTE ////
		{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
		//// COLOR ATTRIBUTE ////
		{"COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0,	12,	D3D11_INPUT_PER_VERTEX_DATA, 0}
	};
	*/
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

pimm::BinaryData pimm::VertexShaderSignature::GetShaderBinaryData() const noexcept
{
	return m_vertexShaderBinary->GetData();
}

pimm::BinaryData pimm::VertexShaderSignature::GetInputElementsData() const noexcept
{
	return
	{
		m_elements,
		m_numberOfElements
	};
}
