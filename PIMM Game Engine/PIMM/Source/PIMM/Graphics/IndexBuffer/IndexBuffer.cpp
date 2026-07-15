#include <PIMM/Graphics/IndexBuffer/IndexBuffer.h>

pimm::IndexBuffer::IndexBuffer(const IndexBufferDescriptor& indexBufferDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor) :
	GraphicsResource(graphicsResourceDescriptor),
	m_size(indexBufferDescriptor.indexListSize)
{
	if (!indexBufferDescriptor.indexList) PIMMLogThrowInvalidArgument("No index list provided");
	if (!indexBufferDescriptor.indexListSize) PIMMLogThrowInvalidArgument("Index list size must be non-zero");

	D3D11_BUFFER_DESC bufferDescriptor{};

	bufferDescriptor.ByteWidth = indexBufferDescriptor.indexListSize * sizeof(ui32);
	bufferDescriptor.BindFlags = D3D11_BIND_INDEX_BUFFER;

	D3D11_SUBRESOURCE_DATA initialData{};
	initialData.pSysMem = indexBufferDescriptor.indexList;

	//Create Buffer allocates a buffer in GPU memory, whether its vertex buffer, index buffer, or constant buffer
	PIMMGraphicsLogThrowOnFail(
		m_d3dDevice.CreateBuffer
		(
			&bufferDescriptor,	//Pointer to a buffer descriptor. A structure that describes how the GPU buffer should be created
			&initialData,		//D3D11 Subresource data. Provides a way to supply initial CPU side data to the GPU resource during creation 
			&m_buffer			//Output parameter. Hold the final buffer object
		), "CreateBuffer() failed.");
}

pimm::ui32 pimm::IndexBuffer::GetIndexListSize() const noexcept
{
	return m_size;
}
