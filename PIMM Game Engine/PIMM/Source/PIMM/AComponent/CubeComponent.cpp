#include <PIMM/AComponent/CubeComponent.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::CubeComponent::CubeComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
	//COLOR
	//const Vertex cubeVertices[] =
	//{
	//	{ {-0.5f,-0.5f,-0.5f},	{1,0,0,1}},
	//	{ {-0.5f,0.5f,-0.5f},	{0,1,0,1}},
	//	{ {0.5f,0.5f,-0.5f},	{0,0,1,1}},
	//	{ {0.5f,-0.5f,-0.5f},	{1,1,1,1}},
	//
	//	{ {0.5f,-0.5f,0.5f},	{0,0,0,1}},
	//	{ {0.5f,0.5f,0.5f},		{1,0,1,1}},
	//	{ {-0.5f,0.5f,0.5f},	{0,1,1,1}},
	//	{ {-0.5f,-0.5f,0.5f},	{1,1,0,1}}
	//};

	//UV
	const Vertex cubeVertices[] =
	{
		{ {-0.5f,-0.5f,-0.5f},	{0.0f, 1.0f}},
		{ {-0.5f,0.5f,-0.5f},	{0.0f, 0.0f}},
		{ {0.5f,0.5f,-0.5f},	{1.0f, 0.0f}},
		{ {0.5f,-0.5f,-0.5f},	{1.0f, 1.0f}},

		{ {0.5f,-0.5f,0.5f},	{0.0f, 1.0f}},
		{ {0.5f,0.5f,0.5f},		{0.0f, 0.0f}},
		{ {-0.5f,0.5f,0.5f},	{1.0f, 0.0f}},
		{ {-0.5f,-0.5f,0.5f},	{1.0f, 1.0f}}
	};

	//We are drawing in 4 control point patches
	const ui32 cubeIndices[] =
	{
		//Front Face
		0, 1, 3, 2,
		//Back Face
		4, 5, 7, 6,
		//Top Face
		5, 2, 6, 1,
		//Bottom Face
		7, 0, 4, 3,
		//Right Face
		3, 2, 4, 5,
		//Left Face
		7, 6, 0, 1
	};

	static const auto vb = m_context.graphicsDevice.CreateVertexBuffer
	({
		cubeVertices,
		std::size(cubeVertices),
		sizeof(Vertex)
	});

	static const auto ib = m_context.graphicsDevice.CreateIndexBuffer
	({
		cubeIndices,//Index List
		std::size(cubeIndices)//Index List Size
	});

	m_vertexBuffer = vb;
	m_indexBuffer = ib;
}

pimm::VertexBuffer& pimm::CubeComponent::GetVertexBuffer()
{
	return *m_vertexBuffer;
}

pimm::IndexBuffer& pimm::CubeComponent::GetIndexBuffer()
{
	return *m_indexBuffer;
}
