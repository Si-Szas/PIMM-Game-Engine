#include "MainGame.h"
#include "Player/Player.h"
#include <PIMM/Graphics/ImGui/imgui.h>
using namespace pimm;

MainGame::MainGame(const GameDescriptor& descriptor) :
	Game(descriptor)
{
}

void MainGame::OnCreate()
{
	Game::OnCreate();
	auto& world = GetWorld();

	auto basicMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (basicMaterial)
	{
		auto materialData = pimm::Vec3(1.0f, 1.0f, 1.0f);
		basicMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	auto blueMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (blueMaterial)
	{
		auto materialData = pimm::Vec3(0.0f, 0.0f, 1.0f);
		blueMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	auto purpleMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/Basic.hlsl");
	if (purpleMaterial)
	{
		auto materialData = pimm::Vec3(1.0f, 0.0f, 1.0f);
		purpleMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	auto player = world.CreateAGameObject<Player>();
	player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });

	auto floor = world.CreateAGameObject<pimm::Quad>();
	floor->GetTransform().SetScale({ 20.0f, 1.0f, 20.0f });
	floor->GetTransform().SetPosition({ 0.0f, -10.0f, 0.0f });
	floor->GetMaterialComponent().SetMaterial(basicMaterial);

	auto cube = world.CreateAGameObject<pimm::Cube>();
	cube->GetTransform().SetScale({ 1.0f });
	cube->GetTransform().SetPosition({ 0.0f, 0.0f, 0.0f });
	cube->GetMaterialComponent().SetMaterial(blueMaterial);

	auto sphere = world.CreateAGameObject<pimm::Sphere>();
	sphere->GetTransform().SetScale({ 1.0f });
	sphere->GetTransform().SetPosition({ 0.0f, 0.0f, 6.0f });
	sphere->GetMaterialComponent().SetMaterial(purpleMaterial);

	auto cylinder = world.CreateAGameObject<pimm::Cylinder>();
	cylinder->GetTransform().SetScale({ 1.0f });
	cylinder->GetTransform().SetPosition({ 0.0f, 0.0f, 2.0f });
	cylinder->GetMaterialComponent().SetMaterial(purpleMaterial);

	auto capsule = world.CreateAGameObject<pimm::Capsule>();
	capsule->GetTransform().SetScale({ 1.0f });
	capsule->GetTransform().SetPosition({ 0.0f, 0.0f, 4.0f });
	capsule->GetMaterialComponent().SetMaterial(blueMaterial);

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
