#include <PIMM/AComponent/QuadComponent.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::QuadComponent::QuadComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{

	//const Vertex quadVertices[] =
	//{
	//	/* BR */ { {-1.0f, 0.0f,  -1.0f},  {1.0f, 1.0f, 1.0f, 1.0f} },
	//	/* BL */ { {-1.0f, 0.0f,   1.0f }, {1.0f, 1.0f, 0.0f, 1.0f}},
	//	/* TR */ { {1.0f,  0.0f,  -1.0f},  {1.0f, 0.0f, 1.0f, 1.0f} },
	//	/* TL */ { {1.0f,  0.0f,   1.0f},  {0.0f, 1.0f, 1.0f, 1.0f} }
	//};
	//UVS
	const Vertex quadVertices[] =
	{
		/* BR */ { {-1.0f, 0.0f, -1.0f},   {1.0f, 1.0f} },
		/* BL */ { {-1.0f, 0.0f,  1.0f},   {0.0f, 1.0f} },
		/* TR */ { { 1.0f, 0.0f, -1.0f},   {1.0f, 0.0f} },
		/* TL */ { { 1.0f, 0.0f,  1.0f},   {0.0f, 0.0f} }
	};

	const ui32 quadIndices[] =
	{
		0, 1, 2, 3
	};

	static const auto vb = m_context.graphicsDevice.CreateVertexBuffer
	({ 
		quadVertices,					//Vertex List
		std::size(quadVertices),		//Vertex List Size
		sizeof(Vertex)				//Vertex Size
	});

	static const auto ib = m_context.graphicsDevice.CreateIndexBuffer
	({
		quadIndices,//Index List
		std::size(quadIndices)//Index List Size
	});

	m_vertexBuffer = vb;
	m_indexBuffer = ib;
}

pimm::VertexBuffer& pimm::QuadComponent::GetVertexBuffer()
{
	return *m_vertexBuffer;
}

pimm::IndexBuffer& pimm::QuadComponent::GetIndexBuffer()
{
	return *m_indexBuffer;
}
