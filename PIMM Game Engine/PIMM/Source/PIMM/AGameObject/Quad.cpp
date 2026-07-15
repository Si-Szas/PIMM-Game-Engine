#include <PIMM/AGameObject/Quad.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::Quad::Quad(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Quad::OnCreate() 
{

	const Vertex quadVertices[] =
	{
		/* BR */ { {-1.0f, 0.0f,  -1.0f},  {1.0f, 1.0f, 1.0f, 1.0f} },
		/* BL */ { {-1.0f, 0.0f,   1.0f }, {1.0f, 1.0f, 0.0f, 1.0f}},
		/* TR */ { {1.0f,  0.0f,  -1.0f},  {1.0f, 0.0f, 1.0f, 1.0f} },
		/* TL */ { {1.0f,  0.0f,   1.0f},  {0.0f, 1.0f, 1.0f, 1.0f} }
	};

	const ui32 quadIndices[] =
	{
		0, 1, 2, 3
	};

	auto& worldRenderer = GetWorldRenderer();
	auto& device = worldRenderer.GetGraphicsDevice();

	m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
	m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

	worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
	({
		quadVertices,					//Vertex List
		std::size(quadVertices),		//Vertex List Size
		sizeof(Vertex)				//Vertex Size
		}));

	worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
	({
		quadIndices,//Index List
		std::size(quadIndices)//Index List Size
		}));

	m_quadComponent = CreateOrGetComponent<QuadComponent>();
}

void pimm::Quad::OnUpdate(f32 deltaTime) {

}

pimm::Quad::~Quad() {

}