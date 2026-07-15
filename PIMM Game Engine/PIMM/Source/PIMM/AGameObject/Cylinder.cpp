#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::Cylinder::Cylinder(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Cylinder::OnCreate()
{
    std::vector<Vertex> cylinderVertices;
    //Push bottommost vertex of cylinder
    ui32 bottomCenterIndex = static_cast<ui32>(cylinderVertices.size());
    cylinderVertices.push_back({
        { 0.0f, -(height * 0.5f), 0.0f },
        { 0.0f, 0.0f, 0.0f, 1.0f }
        });

    //Cylinder body / ring
    for (ui32 i = 0; i <= stackCount; i++)
    {
        f32 y = ((f32)i / stackCount) * height - (height * 0.5f);

        for (ui32 j = 0; j <= sliceCount; j++)
        {
            // Calculate angle around the cylinder
            f32 theta = ((f32)j / sliceCount) * MathUtility::PI2;

            f32 x = radius * cosf(theta);
            f32 z = radius * sinf(theta);

            f32 r = (x / (2.0f * radius)) + 0.5f;
            f32 g = (y / height) + 0.5f;
            f32 b = (z / (2.0f * radius)) + 0.5f;

            cylinderVertices.push_back({
                {x, y, z},
                {r, g, b, 1.0f}
                });
        }
    }

    //Push topmost vertex of cylinder
    ui32 topCenterIndex = static_cast<ui32>(cylinderVertices.size());
    cylinderVertices.push_back({
        { 0.0f, (height * 0.5f), 0.0f },
        { 1.0f, 1.0f, 1.0f, 1.0f }
        });

    //Create indices
    std::vector<ui32> cylinderIndices;
    //Bottom vertex indices
    ui32 ringVertexCount = sliceCount + 1;
    for (ui32 i = 0; i < sliceCount; i++)
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
    for (ui32 i = 0; i < stackCount; i++)
    {
        for (ui32 j = 0; j < sliceCount; j++)
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
    ui32 topStart = (stackCount * ringVertexCount) + 1;
    for (ui32 i = 0; i < sliceCount; i++)
    {
        ui32 topCurrent = topStart + i;
        ui32 topNext = topStart + i + 1;

        cylinderIndices.push_back(topCurrent);
        cylinderIndices.push_back(topCenterIndex);
        cylinderIndices.push_back(topNext);
        cylinderIndices.push_back(topCenterIndex);
    }

    auto& worldRenderer = GetWorldRenderer();
    auto& device = worldRenderer.GetGraphicsDevice();

    m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
    m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

    worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
    ({
        cylinderVertices.data(),
        static_cast<UINT>(cylinderVertices.size()),
        sizeof(Vertex)
        }));

    worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
    ({
        cylinderIndices.data(),
        static_cast<UINT>(cylinderIndices.size())
        }));

    m_cylinderComponent = CreateOrGetComponent<CylinderComponent>();
}

void pimm::Cylinder::OnUpdate(f32 deltaTime)
{

}

pimm::Cylinder::~Cylinder()
{

}
