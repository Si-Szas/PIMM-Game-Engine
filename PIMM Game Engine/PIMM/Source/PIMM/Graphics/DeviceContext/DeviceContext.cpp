#include <PIMM/Graphics/DeviceContext/DeviceContext.h>
#include <PIMM/Graphics/SwapChain/SwapChain.h>
#include <PIMM/Graphics/GraphicsPipelineState/GraphicsPipelineState.h>
#include <PIMM/Graphics/VertexBuffer/VertexBuffer.h>
#include <PIMM/Graphics/ConstantBuffer/ConstantBuffer.h>
#include <PIMM/Graphics/IndexBuffer/IndexBuffer.h>
#include <PIMM/Graphics/Texture/Texture.h>
#include <PIMM/Graphics/Sampler/Sampler.h>

#include <ranges>

#include <wrl.h>

pimm::DeviceContext::DeviceContext(const GraphicsResourceDescriptor& descriptor) :
	GraphicsResource(descriptor)
{
	PIMMGraphicsLogThrowOnFail(
	m_d3dDevice.CreateDeferredContext(
		0,		//Reserved parameter that doesn't affect deferred context
		&m_context	//Output parameter where we receive D311 Device instance
	), "CreateDeferredContext() failed.");
}

void pimm::DeviceContext::ClearAndSetBackBuffer(const SwapChain& swapChain, const Vec4& color)
{
	//Clear the content of the render target view
	//Clear and fill back buffer with a specific color

	//Should be in a range of 0 to 1
	f32 colorArray[] = {color.x, color.y, color.z, color.w};
	
	auto RTV = swapChain.m_renderTargetView.Get();
	auto DSV = swapChain.m_depthStencilView.Get();

	m_context->ClearRenderTargetView(
		RTV,	//RTV, object representing view into a so-called render target
		colorArray							//Array of 4 float values representing RGBA	
	);
	
	m_context->ClearDepthStencilView(
		DSV,
		D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL,
		1,
		0
	);

	//Call OMSetRenderTarget to bind buffer we want to render graphics into
	m_context->OMSetRenderTargets(
		1,			//Number of render target views (we set all in one view, our back buffer)
		&RTV,		//An array of pointers to the views (we simulate an array using &)
		DSV		//Depth Stencil View
	);
}

void pimm::DeviceContext::ExecuteCommandList(const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& immediateContext)
{
	Microsoft::WRL::ComPtr<ID3D11CommandList> commandList{};

	//Retrieve command list from passed in device context
	auto hr =
		m_context->FinishCommandList
		(
			false,			//Bool flag whether to restore previous graphics pipeline state. Pass false to optimize
			&commandList	//Output parameter where we retrieve command list
		);

	if (FAILED(hr))
	{
		PIMMLogError("FinishCommandList() failed.");
		return;
	} 
	
	if (SUCCEEDED(hr) && commandList)
	{
		immediateContext->ExecuteCommandList(commandList.Get(), FALSE);
	}
}

void pimm::DeviceContext::SetGraphicsPipelineState(const GraphicsPipelineState& pipeline)
{
	//Bind input layout to graphics pipeline. How to interpret bimnary structure of our vertex data and how it maps to vertex shaders expected inputs
	m_context->IASetInputLayout(pipeline.m_inputLayout.Get());

	//Actually binds vertex shader to GPU pipeline (use this vertex shader for the next draw calls)
	//It's like in GDGRAP1 where you had to call what shader you wanted to use before drawing specific things
	m_context->VSSetShader(
		pipeline.m_vertexShader.Get(),	//Get the Vertex Shader in Pipeline
		nullptr,						//Related to dynamic shader linkage
		0								//Related to dynamic shader linkage
	);

	//Set up Hull Shader
	m_context->HSSetShader(
		pipeline.m_hullShader.Get(),
		nullptr,
		0
	);

	//Set up Domain Shader
	m_context->DSSetShader(
		pipeline.m_domainShader.Get(),
		nullptr,
		0
	);

	//Do the same but for pixel shader
	m_context->PSSetShader(
		pipeline.m_pixelShader.Get(),	//Get Pixel Shader in Pipeline
		nullptr,
		0
	);
}

//Define viewport of rectangular region of render target where final image is drawn
void pimm::DeviceContext::SetViewportSize(const Rect& size)
{
	D3D11_VIEWPORT vp{};
	vp.Width = static_cast<f32>(size.width);		//Width of window
	vp.Height = static_cast<f32>(size.height);		//Height of window
	vp.MinDepth = 0.0f;	//Near clipping plane. Depth range to which gepmetry is mapped after project during viewport transformation
	vp.MaxDepth = 1.0f;	//Far clipping plane

	//Context where and how our geometry will be rasterized on the screen
	m_context->RSSetViewports
	(
		1,	//Number of viewports we intent to use
		&vp	//Pointer to D3D11 viewport object
	);
}

void pimm::DeviceContext::SetVertexBuffer(const VertexBuffer& buffer)
{
	//Get list of buffers
	auto buff = buffer.m_buffer.Get();
	//Retrieve vertex size from vertex buffer class
	auto stride = buffer.m_vertexSize;
	//Initialize offset
	auto offset = 0u;

	//Defines one or more vertex buffers to the input assembler stage of the graphics pipeline
	//How we tell where vertex data is stored and how to interpret it
	m_context->IASetVertexBuffers
	(
		0,			//Start slot, starting point in list of vertex buffers
		1,			//Number of buffers being passed
		&buff,		//Actual list of buffers. Pointer to an array of D3D11 buffer pointers
		&stride,	//List of strides. Represent the size of a single vertex in bytes
		&offset		//Offset parameter to indicate where to start processing the data
	);
}

void pimm::DeviceContext::SetConstantBuffers(const std::span<ConstantBuffer*>& buffers)
{
	if (!buffers.size())
	{
		PIMMLogError("No buffer passed to SetConstantBuffers()");
		return;
	}

	if (buffers.size() > MaxConstantBuffersPerScene)
	{
		PIMMLogWarning("Number of buffers exceeds {}. Extra buffers will be ignored.", MaxConstantBuffersPerScene)
	}

	auto numBuffers = static_cast<UINT>(std::min(buffers.size(), MaxConstantBuffersPerScene));
	for (auto i = 0u; i < numBuffers; i++)
	{
		m_constantBuffers[i] = (buffers[i]->m_buffer.Get());
	}

	m_context->VSSetConstantBuffers(0, numBuffers, m_constantBuffers.data());
	m_context->HSSetConstantBuffers(0, numBuffers, m_constantBuffers.data());
	m_context->DSSetConstantBuffers(0, numBuffers, m_constantBuffers.data());
	m_context->PSSetConstantBuffers(0, numBuffers, m_constantBuffers.data());

}

//void pimm::DeviceContext::SetVSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer)
//{
//	if (&buffer)
//	{
//		auto buff = buffer.m_buffer.Get();
//		m_context->VSSetConstantBuffers(startSlot, numberOfBuffers, &buff);
//	}
//}
//
//void pimm::DeviceContext::SetHSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer)
//{
//	if (&buffer)
//	{
//		auto buff = buffer.m_buffer.Get();
//		m_context->HSSetConstantBuffers(startSlot, numberOfBuffers, &buff);
//	}
//}
//
//void pimm::DeviceContext::SetDSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer)
//{
//	if (&buffer)
//	{
//		auto buff = buffer.m_buffer.Get();
//		m_context->DSSetConstantBuffers(startSlot, numberOfBuffers, &buff);
//	}
//}
//
//void pimm::DeviceContext::SetPSConstantBuffer(ui32 startSlot, ui32 numberOfBuffers, const ConstantBuffer& buffer)
//{
//	if (&buffer)
//	{
//		auto buff = buffer.m_buffer.Get();
//		m_context->PSSetConstantBuffers(startSlot, numberOfBuffers, &buff);
//	}
//}

//void pimm::DeviceContext::SetConstantBuffer(const ConstantBuffer& vsConstantBuffer, const ConstantBuffer& psConstantBuffer)
//{
//	//Checkers to ensure that the constant buffer for the vertex and pixel shader exist
//	if (&vsConstantBuffer)
//	{
//		auto vsBuff = vsConstantBuffer.m_buffer.Get();
//		m_context->DSSetConstantBuffers(0, 1, &vsBuff);
//	}
//
//	if (&psConstantBuffer)
//	{
//		auto psBuff = psConstantBuffer.m_buffer.Get();
//		m_context->PSSetConstantBuffers(0, 1, &psBuff);
//	}
//}

void pimm::DeviceContext::SetIndexBuffer(const IndexBuffer& buffer)
{
	auto buff = buffer.m_buffer.Get();

	m_context->IASetIndexBuffer(
		buff,					//Pointer to Buffer
		DXGI_FORMAT_R32_UINT,	//Format of the buffer
		0						//Offset
	);
}

void pimm::DeviceContext::SetTextures(const std::span<Texture*>& textures)
{
	if (textures.size() > MaxTexturesPerScene)
	{
		PIMMLogWarning("Number of textures exceeds {}. Extra textures will be ignored.", MaxTexturesPerScene)
	}

	auto numTextures = static_cast<UINT>(std::min(textures.size(), MaxTexturesPerScene));
	
	for (auto i : std::views::iota(0u, numTextures))
	{
		if (textures[i]) m_shaderResourceView[i] = (textures[i]->m_shaderResourceView.Get());
		else m_shaderResourceView[i] = {};
	}
	
	m_context->VSSetShaderResources(0, numTextures, m_shaderResourceView.data());
	m_context->HSSetShaderResources(0, numTextures, m_shaderResourceView.data());
	m_context->DSSetShaderResources(0, numTextures, m_shaderResourceView.data());
	m_context->PSSetShaderResources(0, numTextures, m_shaderResourceView.data());
}

void pimm::DeviceContext::SetSamplers(const std::span<Sampler*>& samplers)
{
	if (samplers.size() > MaxSamplersPerScene)
	{
		PIMMLogWarning("Number of samplers exceeds {}. Extra samplers will be ignored.", MaxSamplersPerScene)
	}

	auto numSamplers = static_cast<UINT>(std::min(samplers.size(), MaxSamplersPerScene));
	for (auto i : std::views::iota(0u, numSamplers))
	{
		if (samplers[i]) m_samplers[i] = (samplers[i]->m_sampler.Get());
		else m_samplers[i] = {};
	}
	
	m_context->VSSetSamplers(0, numSamplers, m_samplers.data());
	m_context->HSSetSamplers(0, numSamplers, m_samplers.data());
	m_context->DSSetSamplers(0, numSamplers, m_samplers.data());
	m_context->PSSetSamplers(0, numSamplers, m_samplers.data());
}

Microsoft::WRL::ComPtr<ID3D11DeviceContext> pimm::DeviceContext::GetD3D11DeviceContext()
{
	return m_context;
}


void pimm::DeviceContext::UpdateConstantBuffer(const ConstantBuffer& buffer, const std::span<const std::byte>& data)
{
	//Means the buffer provided is a nullptr
	if (!&buffer) return;

	auto dataSize = static_cast<ui32>(data.size());
	if (!dataSize)
	{
		PIMMLogError("No data passed to UpdateConstantBuffer()");
		return;
	}

	if (dataSize > buffer.m_size)
	{
		PIMMLogWarning("Buffer size ({} bytes) exceeds the constant buffer limit ({} bytes). Extra bytes will be ignored.", dataSize, buffer.m_size);
	}

	dataSize = std::min(dataSize, buffer.m_size);

	auto buff = buffer.m_buffer.Get();

	D3D11_MAPPED_SUBRESOURCE mapped{}; //Tells you how much data can be viewed

	auto hr = m_context->Map
	(
		buff,	//List of constant buffers
		0,		//Start slot, starting point in list
		D3D11_MAP_WRITE_DISCARD, //CPU read and write permissions
		0,		//What CPU does if GPU is busy. Optional
		&mapped //Pointer to structure for mapped subresources
	);

	if (FAILED(hr))
	{
		PIMMLogError("ID3D11DeviceContext::Map failed.");
		return;
	}

	std::memcpy(mapped.pData, data.data(), buffer.m_size);
	m_context->Unmap(buff, 0);
}

void pimm::DeviceContext::DrawTriangleList(ui32 vertexCount, ui32 startVertexLocation)
{
	//How it assembles data into geometric primitives
	//Tells GPU how to connect the vertices
	//Triangle list specifies how the GPU should treat vertex data, every 3 vertices is an independent triangle

	//For tessellation
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Call Draw function
	m_context->Draw
	(
		vertexCount,		//Vertex Count. Defines number of vertices to draw
		startVertexLocation //Start vertex location. Allows us to specify the index of the first index in the vertex buffer to start drawing from
	);
}

void pimm::DeviceContext::DrawTriangleListWithTessellation(ui32 vertexCount, ui32 startVertexLocation)
{
	//For tessellation
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	//Call Draw function
	m_context->Draw
	(
		vertexCount,		//Vertex Count. Defines number of vertices to draw
		startVertexLocation //Start vertex location. Allows us to specify the index of the first index in the vertex buffer to start drawing from
	);
}

void pimm::DeviceContext::DrawQuadList(ui32 vertexCount, ui32 startVertexLocation)
{
	//For tessellation
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//Call Draw function
	m_context->Draw
	(
		vertexCount,		//Vertex Count. Defines number of vertices to draw
		startVertexLocation //Start vertex location. Allows us to specify the index of the first index in the vertex buffer to start drawing from
	);
}

void pimm::DeviceContext::Draw3PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation)
{
	//For tessellation
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_3_CONTROL_POINT_PATCHLIST);

	//Call Draw function
	m_context->DrawIndexed
	(
		indexCount,			//Current index count
		startVertexIndex,	//Starting vertex index
		startIndexLocation	//Starting index
	);
}

void pimm::DeviceContext::Draw4PatchIndexedTriangleList(ui32 indexCount, ui32 startVertexIndex, ui32 startIndexLocation)
{
	//For tessellation
	m_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST);

	//Call Draw function
	m_context->DrawIndexed
	(
		indexCount,			//Current index count
		startVertexIndex,	//Starting vertex index
		startIndexLocation	//Starting index
	);
}
