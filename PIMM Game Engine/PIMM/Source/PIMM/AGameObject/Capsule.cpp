#include <PIMM/AGameObject/Capsule.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Math/MathUtility.h>

pimm::Capsule::Capsule(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Capsule::OnCreate()
{
    std::vector<Vertex> capsuleVertices;
    f32 phiStep = MathUtility::PI / 2;
    f32 thetaStep = MathUtility::PI2 / sliceCount;

    //Half-circle connecting to bottommost vertex
    //Basically using the algorithm to draw a sphere but only half of it
    for (ui32 i = 0; i < stackCount; i++)
    {
        f32 phi = -phiStep + ((f32)i / stackCount) * phiStep;
        f32 bottomYPos = -(height * 0.5f);

        for (ui32 j = 0; j <= sliceCount; j++) {
            f32 theta = j * thetaStep;

            // Compute positions
            f32 x = radius * cosf(phi) * cosf(theta);
            f32 y = radius * sinf(phi) + bottomYPos;
            f32 z = radius * cosf(phi) * sinf(theta);
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
            f32 u = (f32)j / sliceCount;
            f32 v = ((f32)i / stackCount) * 0.3333f;
            
            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Capsule body / ring
    //Uses the same algorithm as making the body of the cylinder
    for (ui32 i = 0; i <= stackCount; i++)
    {
        f32 y = ((f32)i / stackCount) * height - (height * 0.5f);

        for (ui32 j = 0; j <= sliceCount; j++)
        {
            // Calculate angle around the capsule
            f32 theta = ((f32)j / sliceCount) * MathUtility::PI2;

            f32 x = radius * cosf(theta);
            f32 z = radius * sinf(theta);
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
            f32 u = (f32)j / sliceCount;
            f32 v = 0.3333f + (((f32)i / stackCount) * 0.3333f);

            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Half-circle connecting to topmost vertex
    for (ui32 i = 1; i <= stackCount; i++) {
        f32 phi = ((f32)i / stackCount) * phiStep;
        f32 topYPos = height * 0.5f;

        for (ui32 j = 0; j <= sliceCount; j++) {
            f32 theta = j * thetaStep;

            // Compute positions
            f32 x = radius * cosf(phi) * cosf(theta);
            f32 y = radius * sinf(phi) + topYPos;
            f32 z = radius * cosf(phi) * sinf(theta);
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
            f32 u = (f32)j / sliceCount;
            f32 v = 0.6666f + (((f32)i / stackCount) * 0.3333f);

            capsuleVertices.push_back({
                {x, y, z},
                {u, v}
                });
        }
    }

    //Create indices
    std::vector<ui32> capsuleIndices;
    ui32 ringVertexCount = sliceCount + 1;
    ui32 totalStackCount = stackCount * 3;
    for (ui32 i = 0; i < totalStackCount; i++)
    {
        for (ui32 j = 0; j < sliceCount; j++)
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

    auto& worldRenderer = GetWorldRenderer();
    auto& device = worldRenderer.GetGraphicsDevice();

    m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
    m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

    worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
    ({
        capsuleVertices.data(),					//Vertex List
        static_cast<UINT>(capsuleVertices.size()),		//Vertex List Size
        sizeof(Vertex)				//Vertex Size
        }));

    worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
    ({
        capsuleIndices.data(),//Index List
        static_cast<UINT>(capsuleIndices.size())//Index List Size
        }));

    m_capsuleComponent = CreateOrGetComponent<CapsuleComponent>();
    //m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Capsule::OnUpdate(f32 deltaTime)
{

}

pimm::Capsule::~Capsule()
{

}