#include <PIMM/AComponent/SphereComponent.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Math/MathUtility.h>

#include <vector>

pimm::SphereComponent::SphereComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
	std::vector<Vertex> sphereVertices;
	//Push back the topmost vertex of the sphere
	sphereVertices.push_back(
		{ { 0.0f, m_radius, 0.0f },
		//	{ 1.0f, 1.0f, 1.0f, 1.0f }
			{ 0.5f, 0.0f }
		});

	f32 phiStep = MathUtility::PI / m_stackCount;
	f32 thetaStep = MathUtility::PI2 / m_sliceCount;

	for (ui32 i = 1; i < m_stackCount; i++) {
		f32 phi = i * phiStep;

		f32 v = (f32)i / m_stackCount;

		for (ui32 j = 0; j <= m_sliceCount; j++) {
			f32 theta = j * thetaStep;

			// Compute positions
			f32 x = m_radius * std::sin(phi) * std::cos(theta);
			f32 y = m_radius * std::cos(phi);
			f32 z = m_radius * std::sin(phi) * std::sin(theta);
			// Compute colors
			//f32 r = (x / radius) * 0.5f + 0.5f;
			//f32 g = (y / radius) * 0.5f + 0.5f;
			//f32 b = (z / radius) * 0.5f + 0.5f;
			//
			//sphereVertices.push_back({
			//	{x, y, z},
			//	{r, g, b, 1.0f}
			//	});
			// Compute uv
			f32 u = (f32)j / m_sliceCount;

			sphereVertices.push_back({
				{x, y, z},
				{u, v}
				});
		}
	}
	//Push back the bottommost vertex of the sphere
	sphereVertices.push_back(
		{ { 0.0f, -m_radius, 0.0f },
		//{ 0.0f, 0.0f, 0.0f, 1.0f }
		{ 0.5f, 1.0f }
		});

	//Create index buffer
	std::vector<ui32> sphereIndices;
	//Top vertex indices
	for (ui32 i = 0; i < m_stackCount; i++) {
		for (ui32 j = 0; j < m_sliceCount; j++) {
			sphereIndices.push_back(0);
			sphereIndices.push_back(0);
			sphereIndices.push_back(j + 1);
			sphereIndices.push_back(j + 2);
		}
	}

	// Inner ring quads
	ui32 ringVertexCount = m_sliceCount + 1;
	for (ui32 i = 0; i < m_stackCount - 2; i++) {
		for (ui32 j = 0; j < m_sliceCount; j++) {
			// Calculate quad corners
			ui32 bottomLeft = 1 + i * ringVertexCount + j;
			ui32 bottomRight = bottomLeft + 1;
			ui32 topLeft = 1 + (i + 1) * ringVertexCount + j;
			ui32 topRight = topLeft + 1;

			// 4 indices pushed since domain shader working with quad patches
			sphereIndices.push_back(bottomLeft);
			sphereIndices.push_back(bottomRight);
			sphereIndices.push_back(topLeft);
			sphereIndices.push_back(topRight);
		}
	}

	// Bottom vertex indices
	ui32 southPoleIndex = (ui32)sphereVertices.size() - 1;

	for (ui32 j = 0; j < m_sliceCount; j++) {
		sphereIndices.push_back((southPoleIndex - ringVertexCount) + j);
		sphereIndices.push_back((southPoleIndex - ringVertexCount) + j + 1);
		sphereIndices.push_back(southPoleIndex);
		sphereIndices.push_back(southPoleIndex);
	}

	static const auto vb = m_context.graphicsDevice.CreateVertexBuffer
	({
		sphereVertices.data(),					//Vertex List
		static_cast<UINT>(sphereVertices.size()),		//Vertex List Size
		sizeof(Vertex)				//Vertex Size
	});

	static const auto ib = m_context.graphicsDevice.CreateIndexBuffer
	({
		sphereIndices.data(),//Index List
		static_cast<UINT>(sphereIndices.size())//Index List Size
	});

	m_vertexBuffer = vb;
	m_indexBuffer = ib;
}

pimm::VertexBuffer& pimm::SphereComponent::GetVertexBuffer()
{
	return *m_vertexBuffer;
}

pimm::IndexBuffer& pimm::SphereComponent::GetIndexBuffer()
{
	return *m_indexBuffer;
}

pimm::f32 pimm::SphereComponent::GetRadius()
{
	return m_radius;
}

//void pimm::SphereComponent::SetRadius(f32 newRadius)
//{
//	radius = newRadius;
//}
