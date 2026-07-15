#include <PIMM/InputSystem/Commands/CreateAGameObjectCommand.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/Math/Vec3.h>
#include <random>

pimm::CreateAGameObjectCommand::CreateAGameObjectCommand(const InputSystemDescriptor& descriptor) :
	InputCommand(descriptor)
{
}

void pimm::CreateAGameObjectCommand::ExecuteCommand()
{
	//static std::random_device randDevice;
	//static std::mt19937 generator(randDevice());
	//std::uniform_real_distribution<f32> dis(-0.5f, 0.5f);
	//auto sphere = m_world->CreateAGameObject<pimm::Sphere>();
	//sphere->GetTransform().SetScale({ 1.0f });
	//sphere->GetTransform().SetPosition({dis(generator), dis(generator), 0.0f});
	//
	////random direction here
	//Vec3 movementDir{ dis(generator), dis(generator), 0.0f };
	//if (pimm::Vec3::LengthSquared(movementDir) < 0.001f)
	//{
	//	movementDir = Vec3(1.0f, 0.0f, 0.0f);
	//}
	//
	//Vec3::Normalize(movementDir);
	//sphere->SetVelocity(movementDir);
}

void pimm::CreateAGameObjectCommand::ExecuteCommand(AGameObject& gameObject, World& world)
{
	static std::random_device randDevice;
	static std::mt19937 generator(randDevice());
	std::uniform_real_distribution<f32> dis(-5.0f, 5.0f);

	auto sphere = world.CreateAGameObject<pimm::Sphere>();
	sphere->GetTransform().SetScale({ 1.0f });
	sphere->GetTransform().SetPosition({dis(generator), dis(generator), dis(generator)});

}

void pimm::CreateAGameObjectCommand::UndoCommand(AGameObject& gameObject)
{

}

pimm::CreateAGameObjectCommand::~CreateAGameObjectCommand()
{

}