#include <PIMM/AGameObject/Cube.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::Cube::Cube(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Cube::OnCreate() 
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

	auto& worldRenderer = GetWorldRenderer();
	auto& device = worldRenderer.GetGraphicsDevice();

	m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
	m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

	worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
	({
		cubeVertices,
		std::size(cubeVertices),
		sizeof(Vertex)
		}));

	worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
	({
		cubeIndices,//Index List
		std::size(cubeIndices)//Index List Size
		}));

	m_cubeComponent = CreateOrGetComponent<CubeComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Cube::OnUpdate(f32 deltaTime)
{
}

pimm::Cube::~Cube()
{
}
