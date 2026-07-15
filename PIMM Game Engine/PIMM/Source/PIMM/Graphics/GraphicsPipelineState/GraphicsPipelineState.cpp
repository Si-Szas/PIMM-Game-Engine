#include <PIMM/Graphics/GraphicsPipelineState/GraphicsPipelineState.h>
#include <PIMM/Graphics/ShaderBinary/ShaderBinary.h>
#include <PIMM/Graphics/VertexShaderSignature/VertexShaderSignature.h>

pimm::GraphicsPipelineState::GraphicsPipelineState(const GraphicsPipelineStateDescriptor& graphicsPipelineStateDescriptor, const GraphicsResourceDescriptor& descriptor):
	GraphicsResource(descriptor)
{
	//Ensures that the shader type of the vertex shader is actually a vertex shader
	if (graphicsPipelineStateDescriptor.pixelShader.GetShaderType() != ShaderType::PixelShader)
		PIMMLogThrowInvalidArgument("The 'pixelShader' member is not a valid pixel shader binary.");

	//Retrieve Vertex Shader Binary Data
	auto vs = graphicsPipelineStateDescriptor.vertexShader.GetShaderBinaryData();
	//Retrieve Pixel Shader Binary Data
	auto ps = graphicsPipelineStateDescriptor.pixelShader.GetData();
	//Retrieve Hull Shader Binary Data
	auto hs = graphicsPipelineStateDescriptor.hullShader.GetData();
	////Retrieve Domain Shader Binary Data
	auto ds = graphicsPipelineStateDescriptor.domainShader.GetData();

	auto vsInputElements = graphicsPipelineStateDescriptor.vertexShader.GetInputElementsData();

	//Create Input Layout
	PIMMGraphicsLogThrowOnFail(
	m_d3dDevice.CreateInputLayout
	(
		static_cast<const D3D11_INPUT_ELEMENT_DESC*>(vsInputElements.data),	//Pointer to a list of D3D11 Input Elements desc objects
		static_cast<ui32>(vsInputElements.dataSize),	//Size of input elements
		vs.data,					//Shader byte code. Defines vertex data structure, which is defined in vertex shader
		vs.dataSize,
		&m_inputLayout				//Output parameter. Input layout
	), "CreateInputLayout() failed.");

	//Get a reference to the D3D11 Device and call its CreateVertexShader
		//This method creates a vertex shader object from bytecode (takes HLSL bytecode)
		// Returns a d3d11 vertex shader object that can be bound to gpu pipeline)
	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreateVertexShader
		(
			vs.data,		//Shader byte code (Pointer to binary data)
			vs.dataSize,	//Size of shader byte code
			nullptr,		//Pointer to a D3D11 class linkage object. Interface used to enable polymorphism by HLSL interface & classes (dynamic implementation binding)
			&m_vertexShader	//Output parameter where created D3D11 Shader object will be stored
		),
		"CreateVertexShader() failed."
	);

	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreatePixelShader
		(
			ps.data,		//Shader byte code (Pointer to binary data)
			ps.dataSize,	//Size of shader byte code
			nullptr,		//Pointer to a D3D11 class linkage object. Interface used to enable polymorphism by HLSL interface & classes (dynamic implementation binding)
			&m_pixelShader	//Output parameter where created D3D11 Shader object will be stored
		),
		"CreatePixelShader() failed."
	);

	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreateHullShader
		(
			hs.data,
			hs.dataSize,
			nullptr,
			&m_hullShader
		),
		"CreateHullShader() failed."
	);

	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreateDomainShader
		(
			ds.data,
			ds.dataSize,
			nullptr,
			&m_domainShader
		),
		"CreateDomainShader() failed."
	);
}
