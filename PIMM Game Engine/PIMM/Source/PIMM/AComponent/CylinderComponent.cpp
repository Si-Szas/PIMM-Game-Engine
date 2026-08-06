#include <PIMM/AComponent/CylinderComponent.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Math/MathUtility.h>

#include <vector>

pimm::CylinderComponent::CylinderComponent(const AComponentDescriptor& descriptor) :
	AComponent(descriptor)
{
    std::vector<Vertex> cylinderVertices;
    //Push bottommost vertex of cylinder
    ui32 bottomCenterIndex = static_cast<ui32>(cylinderVertices.size());
    cylinderVertices.push_back({
        { 0.0f, -(m_height * 0.5f), 0.0f },
        //{ 0.0f, 0.0f, 0.0f, 1.0f }
        { 0.5f, 1.0f }
        });

    //Cylinder body / ring
    for (ui32 i = 0; i <= m_stackCount; i++)
    {
        f32 y = ((f32)i / m_stackCount) * m_height - (m_height * 0.5f);

        for (ui32 j = 0; j <= m_sliceCount; j++)
        {
            // Calculate angle around the cylinder
            f32 theta = ((f32)j / m_sliceCount) * MathUtility::PI2;

            f32 x = m_radius * cosf(theta);
            f32 z = m_radius * sinf(theta);

            //COMPUTE COLORS
            //f32 r = (x / (2.0f * radius)) + 0.5f;
            //f32 g = (y / height) + 0.5f;
            //f32 b = (z / (2.0f * radius)) + 0.5f;
            //
            //cylinderVertices.push_back({
            //    {x, y, z},
            //    {r, g, b, 1.0f}
            //    });

            //COMPUTE UV
            f32 u = (f32)j / m_sliceCount;
            f32 v = 0.3333f + (((f32)i / m_stackCount) * 0.3333f);

            cylinderVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Push topmost vertex of cylinder
    ui32 topCenterIndex = static_cast<ui32>(cylinderVertices.size());
    cylinderVertices.push_back({
        { 0.0f, (m_height * 0.5f), 0.0f },
        //{ 1.0f, 1.0f, 1.0f, 1.0f }
        { 0.5f, 0.0f }
        });

    //Create indices
    std::vector<ui32> cylinderIndices;
    //Bottom vertex indices
    ui32 ringVertexCount = m_sliceCount + 1;
    for (ui32 i = 0; i < m_sliceCount; i++)
    {
        ui32 bottomCurrent = i + 1;
        ui32 bottomNext = i + 2;

        //Push back the bottom of the cylinder's quads
        cylinderIndices.push_back(bottomNext);
        cylinderIndices.push_back(bottomCenterIndex);
        cylinderIndices.push_back(bottomCurrent);
        cylinderIndices.push_back(bottomCenterIndex);
    }

    //Ring vertex indices
    for (ui32 i = 0; i < m_stackCount; i++)
    {
        for (ui32 j = 0; j < m_sliceCount; j++)
        {
            //Calculate 4 corners of patch
            ui32 bottomLeft = (i * ringVertexCount + j) + 1;
            ui32 bottomRight = bottomLeft + 1;
            ui32 topLeft = ((i + 1) * ringVertexCount + j) + 1;
            ui32 topRight = topLeft + 1;
            // 4 indices pushed since domain shader working with quad patches
            cylinderIndices.push_back(bottomLeft);
            cylinderIndices.push_back(topLeft);
            cylinderIndices.push_back(bottomRight);
            cylinderIndices.push_back(topRight);
        }
    }

    //Top vertex indices
    ui32 topStart = (m_stackCount * ringVertexCount) + 1;
    for (ui32 i = 0; i < m_sliceCount; i++)
    {
        ui32 topCurrent = topStart + i;
        ui32 topNext = topStart + i + 1;

        cylinderIndices.push_back(topCurrent);
        cylinderIndices.push_back(topCenterIndex);
        cylinderIndices.push_back(topNext);
        cylinderIndices.push_back(topCenterIndex);
    }

    static const auto vb = m_context.graphicsDevice.CreateVertexBuffer
    ({
        cylinderVertices.data(),
        static_cast<UINT>(cylinderVertices.size()),
        sizeof(Vertex)
    });

    static const auto ib = m_context.graphicsDevice.CreateIndexBuffer
    ({
        cylinderIndices.data(),
        static_cast<UINT>(cylinderIndices.size())
    });

    m_vertexBuffer = vb;
    m_indexBuffer = ib;
}

pimm::VertexBuffer& pimm::CylinderComponent::GetVertexBuffer()
{
	return *m_vertexBuffer;
}

pimm::IndexBuffer& pimm::CylinderComponent::GetIndexBuffer()
{
	return *m_indexBuffer;
}
