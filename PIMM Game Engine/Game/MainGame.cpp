#include "MainGame.h"
#include "Player/Player.h"

using namespace pimm;

MainGame::MainGame(const GameDescriptor& descriptor) :
	Game(descriptor)
{
}

void MainGame::OnCreate()
{
	Game::OnCreate();
	auto& world = GetWorld();

	auto player = world.CreateAGameObject<Player>();
	player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });

	auto floor = world.CreateAGameObject<pimm::Quad>();
	floor->GetTransform().SetScale({ 20.0f, 1.0f, 20.0f });
	floor->GetTransform().SetPosition({ 0.0f, -10.0f, 0.0f });
	
	auto cube = world.CreateAGameObject<pimm::Cube>();
	cube->GetTransform().SetScale({ 1.0f });
	cube->GetTransform().SetPosition({ 0.0f, 0.0f, 0.0f });
	
	auto sphere = world.CreateAGameObject<pimm::Sphere>();
	sphere->GetTransform().SetScale({ 1.0f });
	sphere->GetTransform().SetPosition({ 0.0f, 0.0f, 6.0f });
	
	auto cylinder = world.CreateAGameObject<pimm::Cylinder>();
	cylinder->GetTransform().SetScale({ 1.0f });
	cylinder->GetTransform().SetPosition({ 0.0f, 0.0f, 2.0f });
	
	auto capsule = world.CreateAGameObject<pimm::Capsule>();
	capsule->GetTransform().SetScale({ 1.0f });
	capsule->GetTransform().SetPosition({ 0.0f, 0.0f, 4.0f });

	//GetInputSystem().SetCursorLocked(true);
	//GetInputSystem().SetCursorVisible(false);
}

void MainGame::OnUpdate(f32 deltaTime)
{
	Game::OnUpdate(deltaTime);
}