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
	auto position = GetTransform().GetPosition();
	auto velocity = GetVelocity();
	auto speed = GetSpeedModifier();

	//Get screen size to know where to bounce
	auto size = GetWorldRenderer().GetSwapChainSize();
	auto aspect = (pimm::f32(size.width)) / (size.height);
	//In the world renderer, units per screen height is 5.0f
	auto halfUnitsPerScreenHeight = 5.0f * 0.5f;
	auto viewHeight = (halfUnitsPerScreenHeight - m_sphereComponent->GetRadius());
	auto viewWidth = (halfUnitsPerScreenHeight * aspect) - m_sphereComponent->GetRadius();

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
