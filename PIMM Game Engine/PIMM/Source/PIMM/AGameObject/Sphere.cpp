#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/Math/MathUtility.h>

#include <iostream>

pimm::Sphere::Sphere(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Sphere::OnCreate()
{
	std::vector<Vertex> sphereVertices;
	//Push back the topmost vertex of the sphere
	sphereVertices.push_back(
		{ { 0.0f, radius, 0.0f },
	//	{ 1.0f, 1.0f, 1.0f, 1.0f }
		{ 0.5f, 0.0f }
		});

	f32 phiStep = MathUtility::PI / stackCount;
	f32 thetaStep = MathUtility::PI2 / sliceCount;

	for (ui32 i = 1; i < stackCount; i++) {
		f32 phi = i * phiStep;

		f32 v = (f32)i / stackCount;

		for (ui32 j = 0; j <= sliceCount; j++) {
			f32 theta = j * thetaStep;

			// Compute positions
			f32 x = radius * std::sin(phi) * std::cos(theta);
			f32 y = radius * std::cos(phi);
			f32 z = radius * std::sin(phi) * std::sin(theta);
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
			f32 u = (f32)j / sliceCount;

			sphereVertices.push_back({
				{x, y, z},
				{u, v}
				});
		}
	}
	//Push back the bottommost vertex of the sphere
	sphereVertices.push_back(
		{ { 0.0f, -radius, 0.0f },
		//{ 0.0f, 0.0f, 0.0f, 1.0f }
		{ 0.5f, 1.0f }
		});

	//Create index buffer
	std::vector<ui32> sphereIndices;
	//Top vertex indices
	for (ui32 i = 0; i < stackCount; i++) {
		for (ui32 j = 0; j < sliceCount; j++) {
			sphereIndices.push_back(0);
			sphereIndices.push_back(0);
			sphereIndices.push_back(j + 1);
			sphereIndices.push_back(j + 2);
		}
	}

	// Inner ring quads
	ui32 ringVertexCount = sliceCount + 1;
	for (ui32 i = 0; i < stackCount - 2; i++) {
		for (ui32 j = 0; j < sliceCount; j++) {
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

	for (ui32 j = 0; j < sliceCount; j++) {
		sphereIndices.push_back((southPoleIndex - ringVertexCount) + j);
		sphereIndices.push_back((southPoleIndex - ringVertexCount) + j + 1);
		sphereIndices.push_back(southPoleIndex);
		sphereIndices.push_back(southPoleIndex);
	}

	auto& worldRenderer = GetWorldRenderer();
	auto& device = worldRenderer.GetGraphicsDevice();

	m_vertexOffset = static_cast<ui32>(worldRenderer.GetVertexBuffer().size());
	m_indexLocation = static_cast<ui32>(worldRenderer.GetIndexBuffer().size());

	worldRenderer.GetVertexBuffer().push_back(device.CreateVertexBuffer
	({
		sphereVertices.data(),					//Vertex List
		static_cast<UINT>(sphereVertices.size()),		//Vertex List Size
		sizeof(Vertex)				//Vertex Size
		}));

	worldRenderer.GetIndexBuffer().push_back(device.CreateIndexBuffer
	({
		sphereIndices.data(),//Index List
		static_cast<UINT>(sphereIndices.size())//Index List Size
		}));

	m_sphereComponent = CreateOrGetComponent<SphereComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Sphere::OnUpdate(f32 deltaTime)
{
	auto position = GetTransform().GetPosition();
	auto velocity = GetVelocity();
	auto speed = GetSpeedModifier();

	//Get screen size to know where to bounce
	auto size = GetWorldRenderer().GetSwapChainSize();
	auto aspect = (pimm::f32(size.width)) / (size.height);
	//In the world renderer, units per screen height is 5.0f
	auto halfUnitsPerScreenHeight = 5.0f * 0.5f;
	auto viewHeight = (halfUnitsPerScreenHeight - radius);
	auto viewWidth = (halfUnitsPerScreenHeight * aspect) - radius;

	position = position + (velocity * speed * deltaTime);

	//If the spheres' position is greater than the view width (screen size)
	if (position.x > viewWidth)
	{
		//Dont let it go over
		position.x = viewWidth;
		//Reverse direction
		velocity.x *= -1.0f;      
	}
	else if (position.x < -viewWidth)
	{
		position.x = -viewWidth;
		velocity.x *= -1.0f;
	}

	if (position.y > viewHeight)
	{
		position.y = viewHeight;
		velocity.y *= -1.0f;      
	}
	else if (position.y < -viewHeight)
	{
		position.y = -viewHeight;
		velocity.y *= -1.0f;
	}

	SetVelocity(velocity);
	GetTransform().SetPosition(position);
}

pimm::Sphere::~Sphere()
{
}
