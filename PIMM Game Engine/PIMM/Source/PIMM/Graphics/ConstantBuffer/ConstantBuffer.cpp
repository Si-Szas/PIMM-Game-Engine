#include <PIMM/Graphics/ConstantBuffer/ConstantBuffer.h>

pimm::ConstantBuffer::ConstantBuffer(const ConstantBufferDescriptor& constantBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor), m_size(constantBufferDescriptor.bufferSize)
{
	if (!constantBufferDescriptor.bufferSize) PIMMLogThrowInvalidArgument("Buffer size invalid, must be non-zero.");

	D3D11_BUFFER_DESC bufferDescriptor{};
	//Dynamic usage because constant values can change during runtime
	//Accessible by GPU and CPU, for resources that will be updated by CPU at least once per frame
	bufferDescriptor.Usage = D3D11_USAGE_DYNAMIC;
	//Defines size of buffer in bytes. Must be greater than 0
	bufferDescriptor.ByteWidth = constantBufferDescriptor.bufferSize;
	//Bind Flags. Since this is a constant buffer, we bind the constant buffer flag
	bufferDescriptor.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	//Mark fllag so that CPU can edit the contents of the resource
	bufferDescriptor.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = constantBufferDescriptor.buffer;

	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreateBuffer
		(
			&bufferDescriptor,											//Pointer to a buffer descriptor. A structure that describes how the GPU buffer should be created
			(constantBufferDescriptor.buffer) ? &initialData:nullptr,	//D3D11 Subresource data. Provides a way to supply initial CPU side data to the GPU resource during creation 
			&m_buffer													//Output parameter. Hold the final buffer object
		), "CreateBuffer() failed.");
}
