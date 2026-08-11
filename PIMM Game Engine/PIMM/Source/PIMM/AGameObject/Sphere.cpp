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
	m_name = "Sphere";
	m_sphereComponent = CreateOrGetComponent<SphereComponent>();
	//m_materialComponent = CreateOrGetComponent<MaterialComponent>();
}

void pimm::Sphere::OnUpdate(f32 deltaTime)
{
}

pimm::Sphere::~Sphere()
{
}
