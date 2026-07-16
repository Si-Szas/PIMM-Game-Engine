#include "MainGame.h"
#include "Player/Player.h"
#include <PIMM/ImGui/imgui.h>
using namespace pimm;

MainGame::MainGame(const GameDescriptor& descriptor) :
	Game(descriptor)
{
}

void MainGame::OnCreate()
{
	Game::OnCreate();
	auto& world = GetWorld();

	auto woodTexture = GetResourceManager().CreateResourceFromFile<pimm::TextureResource>(L"Game/Assets/Textures/wood.jpg");
	auto stoneTexture = GetResourceManager().CreateResourceFromFile<pimm::TextureResource>(L"Game/Assets/Textures/stone.jpg");

	auto basicMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (basicMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		basicMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	auto purpleMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (basicMaterial)
	{
		auto materialData = pimm::Vec3(1.0f, 1.0f, 0.0f);
		purpleMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	auto stoneMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (stoneMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		stoneMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
		stoneMaterial->SetTexture(0, stoneTexture);
	}

	auto woodMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (woodMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		woodMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
		woodMaterial->SetTexture(0, woodTexture);
	}

	auto player = world.CreateAGameObject<Player>();
	player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });
	player->GetTransform().SetRotation({ 20.0f, 0.0f, 0.0f });

	auto floor = world.CreateAGameObject<pimm::Quad>();
	floor->GetTransform().SetScale({ 20.0f, 1.0f, 20.0f });
	floor->GetTransform().SetPosition({ 0.0f, -2.0f, 0.0f });
	floor->GetMaterialComponent().SetMaterial(stoneMaterial);

	auto cube = world.CreateAGameObject<pimm::Cube>();
	cube->GetTransform().SetScale({ 1.0f });
	cube->GetTransform().SetPosition({ 0.0f, 0.0f, 0.0f });
	cube->GetMaterialComponent().SetMaterial(woodMaterial);

	auto sphere = world.CreateAGameObject<pimm::Sphere>();
	sphere->GetTransform().SetScale({ 1.0f });
	sphere->GetTransform().SetPosition({ 2.0f, 0.0f, 5.0f });
	sphere->GetMaterialComponent().SetMaterial(basicMaterial);

	auto cylinder = world.CreateAGameObject<pimm::Cylinder>();
	cylinder->GetTransform().SetScale({ 1.0f });
	cylinder->GetTransform().SetPosition({ 3.0f, 0.0f, 2.0f });
	cylinder->GetMaterialComponent().SetMaterial(purpleMaterial);

	auto capsule = world.CreateAGameObject<pimm::Capsule>();
	capsule->GetTransform().SetScale({ 1.0f });
	capsule->GetTransform().SetPosition({ -2.0f, 0.0f, 3.0f });
	capsule->GetMaterialComponent().SetMaterial(woodMaterial);

	auto* uiManager = GetUIManager();
	uiManager->RegisterPanel(std::make_unique<HierarchyPanel>());
	uiManager->RegisterPanel(std::make_unique<InspectorPanel>());
	uiManager->RegisterPanel(std::make_unique<AssetsPanel>());
}

void MainGame::OnUpdate(f32 deltaTime)
{
	Game::OnUpdate(deltaTime);
}

void MainGame::OnUIRender()
{
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	auto* uiManager = GetUIManager();
	uiManager->RenderPanels();
}
