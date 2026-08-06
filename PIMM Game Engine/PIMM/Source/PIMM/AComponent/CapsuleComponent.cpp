#include <PIMM/AComponent/CapsuleComponent.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Math/MathUtility.h>

#include <vector>

pimm::CapsuleComponent::CapsuleComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
    std::vector<Vertex> capsuleVertices;
    f32 phiStep = MathUtility::PI / 2;
    f32 thetaStep = MathUtility::PI2 / m_sliceCount;

    //Half-circle connecting to bottommost vertex
    //Basically using the algorithm to draw a sphere but only half of it
    for (ui32 i = 0; i < m_stackCount; i++)
    {
        f32 phi = -phiStep + ((f32)i / m_stackCount) * phiStep;
        f32 bottomYPos = -(m_height * 0.5f);

        for (ui32 j = 0; j <= m_sliceCount; j++) {
            f32 theta = j * thetaStep;

            // Compute positions
            f32 x = m_radius * cosf(phi) * cosf(theta);
            f32 y = m_radius * sinf(phi) + bottomYPos;
            f32 z = m_radius * cosf(phi) * sinf(theta);
            // Compute colors
            //f32 r = (x / radius) * 0.5f + 0.5f;
            //f32 g = (y / radius) * 0.5f + 0.5f;
            //f32 b = (z / radius) * 0.5f + 0.5f;
            //
            //capsuleVertices.push_back({
            //    {x, y, z},
            //    {r, g, b, 1.0f}
            //    });
            //Compute UVs
            f32 u = (f32)j / m_sliceCount;
            f32 v = ((f32)i / m_stackCount) * 0.3333f;

            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Capsule body / ring
    //Uses the same algorithm as making the body of the cylinder
    for (ui32 i = 0; i <= m_stackCount; i++)
    {
        f32 y = ((f32)i / m_stackCount) * m_height - (m_height * 0.5f);

        for (ui32 j = 0; j <= m_sliceCount; j++)
        {
            // Calculate angle around the capsule
            f32 theta = ((f32)j / m_sliceCount) * MathUtility::PI2;

            f32 x = m_radius * cosf(theta);
            f32 z = m_radius * sinf(theta);
            //Compute Colors
            //f32 r = (x / (2.0f * radius)) + 0.5f;
            //f32 g = (y / height) + 0.5f;
            //f32 b = (z / (2.0f * radius)) + 0.5f;
            //
            //capsuleVertices.push_back({
            //    {x, y, z},
            //    {r, g, b, 1.0f}
            //    });
            //Compute UVs
            f32 u = (f32)j / m_sliceCount;
            f32 v = 0.3333f + (((f32)i / m_stackCount) * 0.3333f);

            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Half-circle connecting to topmost vertex
    for (ui32 i = 1; i <= m_stackCount; i++) {
        f32 phi = ((f32)i / m_stackCount) * phiStep;
        f32 topYPos = m_height * 0.5f;

        for (ui32 j = 0; j <= m_sliceCount; j++) {
            f32 theta = j * thetaStep;

            // Compute positions
            f32 x = m_radius * cosf(phi) * cosf(theta);
            f32 y = m_radius * sinf(phi) + topYPos;
            f32 z = m_radius * cosf(phi) * sinf(theta);
            // Compute colors
            //f32 r = (x / radius) * 0.5f + 0.5f;
            //f32 g = (y / radius) * 0.5f + 0.5f;
            //f32 b = (z / radius) * 0.5f + 0.5f;
            //
            //capsuleVertices.push_back({
            //    {x, y, z},
            //    {r, g, b, 1.0f}
            //    });
            // Compute UVs
            f32 u = (f32)j / m_sliceCount;
            f32 v = 0.6666f + (((f32)i / m_stackCount) * 0.3333f);

            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Create indices
    std::vector<ui32> capsuleIndices;
    ui32 ringVertexCount = m_sliceCount + 1;
    ui32 totalStackCount = m_stackCount * 3;
    for (ui32 i = 0; i < totalStackCount; i++)
    {
        for (ui32 j = 0; j < m_sliceCount; j++)
        {
            ui32 bottomLeft = i * ringVertexCount + j;
            ui32 bottomRight = bottomLeft + 1;
            ui32 topLeft = (i + 1) * ringVertexCount + j;
            ui32 topRight = topLeft + 1;

            capsuleIndices.push_back(bottomLeft);
            capsuleIndices.push_back(topLeft);
            capsuleIndices.push_back(bottomRight);
            capsuleIndices.push_back(topRight);
        }
    }

    static const auto vb = m_context.graphicsDevice.CreateVertexBuffer
    ({
        capsuleVertices.data(),					//Vertex List
        static_cast<UINT>(capsuleVertices.size()),		//Vertex List Size
        sizeof(Vertex)				//Vertex Size
    });

    static const auto ib = m_context.graphicsDevice.CreateIndexBuffer
    ({
        capsuleIndices.data(),//Index List
        static_cast<UINT>(capsuleIndices.size())//Index List Size
    });

    m_vertexBuffer = vb;
    m_indexBuffer = ib;
}

pimm::VertexBuffer& pimm::CapsuleComponent::GetVertexBuffer()
{
	return *m_vertexBuffer;
}

pimm::IndexBuffer& pimm::CapsuleComponent::GetIndexBuffer()
{
	return *m_indexBuffer;
}

pimm::f32 pimm::CapsuleComponent::GetHeight()
{
	return m_height;
}

pimm::f32 pimm::CapsuleComponent::GetRadius()
{
	return m_radius;
}
