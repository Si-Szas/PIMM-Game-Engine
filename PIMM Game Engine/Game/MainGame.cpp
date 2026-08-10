#include "MainGame.h"
#include "Player/Player.h"
#include "Editor/SceneSerializer.h"
#include <PIMM/ImGui/imgui.h>
#include <PIMM/Graphics/FrameBuffer/FrameBuffer.h>
#include <PIMM/AComponent/RigidBodyComponent.h>

#include "UI/DebugPanel.h"

#include <commdlg.h>
#include <shlobj_core.h>

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
	auto brickTexture = GetResourceManager().CreateResourceFromFile<pimm::TextureResource>(L"Game/Assets/Textures/red_brick.jpg");

	m_basicMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicFourPoint.hlsl");
	if (m_basicMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_basicMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	m_purpleMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicFourPoint.hlsl");
	if (m_purpleMaterial)
	{
		auto materialData = pimm::Vec3(1.0f, 1.0f, 0.0f);
		m_purpleMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	m_stoneMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicFourPoint.hlsl");
	if (m_stoneMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_stoneMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
		m_stoneMaterial->SetTexture(0, stoneTexture);
	}

	m_woodMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicFourPoint.hlsl");
	if (m_woodMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_woodMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
		m_woodMaterial->SetTexture(0, woodTexture);
	}

	m_brickMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicThreePoint.hlsl");
	if (m_brickMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_brickMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
		m_brickMaterial->SetTexture(0, brickTexture);
	}

	auto player = world.CreateAGameObject<Player>();
	player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });
	player->GetTransform().SetRotation({ 20.0f, 0.0f, 0.0f });

	auto floor = world.CreateAGameObject<pimm::Quad>();
	floor->GetTransform().SetScale({ 20.0f, 1.0f, 20.0f });
	floor->GetTransform().SetPosition({ 0.0f, -2.0f, 0.0f });
	floor->GetMaterialComponent().SetMaterial(m_stoneMaterial);

	auto* floorBody = floor->CreateOrGetComponent<pimm::RigidBodyComponent>();
	floorBody->SetBodyType(pimm::BodyType::Static);
	floorBody->AddBoxCollider({ 10.0f, 0.05f, 10.0f });

	//teapot
	{
		auto teapotMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/teapot.obj");

		auto mesh = world.CreateAGameObject<pimm::MeshObject>();
		auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();
		comp->SetMesh(teapotMesh);
		comp->SetMaterial(0, m_brickMaterial);
		mesh->GetTransform().SetPosition({ 0.0f, 1.0f, 0.0f });
		mesh->GetTransform().SetScale({2.0f});
	}

	//bunny
	{
		auto bunnyMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/bunny.obj");

		auto mesh = world.CreateAGameObject<pimm::MeshObject>();
		auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();
		comp->SetMesh(bunnyMesh);
		comp->SetMaterial(0, m_brickMaterial);
		mesh->GetTransform().SetPosition({ 3.0f, 0.0f, 0.0f });
		mesh->GetTransform().SetScale({10.0f});
	}

	//statue
	{
		auto statueMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/statue.obj");

		auto mesh = world.CreateAGameObject<pimm::MeshObject>();
		auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();
		comp->SetMesh(statueMesh);
		comp->SetMaterial(0, m_brickMaterial);
		mesh->GetTransform().SetPosition({ -3.0f, 0.0f, 0.0f });
		mesh->GetTransform().SetScale({5.0f});
	}

	auto* uiManager = GetUIManager();
	uiManager->RegisterPanel(std::make_unique<HierarchyPanel>(world, m_sceneModeManager));
	uiManager->RegisterPanel(std::make_unique<InspectorPanel>(world, m_sceneModeManager));
	uiManager->RegisterPanel(std::make_unique<AssetsPanel>());
	uiManager->RegisterPanel(std::make_unique<ScenePanel>(GetWorldRenderer()));
	uiManager->RegisterPanel(std::make_unique<DebugPanel>(GetLogger()));

	auto topbarPanel = std::make_unique<TopbarPanel>(
		[this](pimm::SpawnObjectType type, bool withPhysics)
		{
			SpawnObject(type, withPhysics);
		},
		[this]()
		{
			m_sceneModeManager.EnterPlayMode(GetWorld());
		},
		[this]()
		{
			m_sceneModeManager.EnterPauseMode(GetWorld());
		},
		[this]()
		{
			m_sceneModeManager.EnterEditMode(GetWorld());
		},
		[this]()
		{
			m_sceneModeManager.RequestFrameStep();
		},
		[this]()
		{
			SaveScene();
		},
		[this]()
		{
			LoadScene();
		}
	);
	m_topbarPanel = topbarPanel.get();
	uiManager->RegisterPanel(std::move(topbarPanel));

	world.SetPhysicsEnabled(false);
}

void MainGame::OnUpdate(f32 deltaTime)
{
	Game::OnUpdate(deltaTime);

	auto& world = GetWorld();

	if (m_sceneModeManager.IsPlayMode())
	{
		world.SetPhysicsEnabled(true);
	}
	else if (m_sceneModeManager.IsPauseMode())
	{
		world.SetPhysicsEnabled(false);
		if (m_sceneModeManager.ConsumeFrameStep())
		{
			world.StepPhysicsFrame();
		}
	}
	else
	{
		world.SetPhysicsEnabled(false);
	}
}

void MainGame::OnUIRender()
{
	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);

	if (m_topbarPanel)
		m_topbarPanel->SetCurrentMode(m_sceneModeManager.GetMode());

	auto* uiManager = GetUIManager();
	uiManager->RenderPanels();
}

void MainGame::SpawnObject(pimm::SpawnObjectType type, bool withPhysics)
{
	auto& world = GetWorld();
	pimm::Vec3 spawnPosition{ 0.0f, 5.0f, 0.0f };

	switch (type)
	{
		case pimm::SpawnObjectType::Cube:
		{
			auto cube = world.CreateAGameObject<pimm::Cube>();
			cube->GetTransform().SetScale({ 1.0f });
			cube->GetTransform().SetPosition(spawnPosition);
			cube->GetMaterialComponent().SetMaterial(m_woodMaterial);
			if (withPhysics)
			{
				auto* body = cube->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(cube->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Sphere:
		{
			auto sphere = world.CreateAGameObject<pimm::Sphere>();
			sphere->GetTransform().SetScale({ 1.0f });
			sphere->GetTransform().SetPosition(spawnPosition);
			sphere->GetMaterialComponent().SetMaterial(m_basicMaterial);
			if (withPhysics)
			{
				auto* body = sphere->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddSphereCollider(0.5f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Cylinder:
		{
			auto cylinder = world.CreateAGameObject<pimm::Cylinder>();
			cylinder->GetTransform().SetScale({ 1.0f });
			cylinder->GetTransform().SetPosition(spawnPosition);
			cylinder->GetMaterialComponent().SetMaterial(m_purpleMaterial);
			if (withPhysics)
			{
				auto* body = cylinder->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddCapsuleCollider(0.5f, 1.0f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Capsule:
		{
			auto capsule = world.CreateAGameObject<pimm::Capsule>();
			capsule->GetTransform().SetScale({ 1.0f });
			capsule->GetTransform().SetPosition(spawnPosition);
			capsule->GetMaterialComponent().SetMaterial(m_woodMaterial);
			if (withPhysics)
			{
				auto* body = capsule->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddCapsuleCollider(0.5f, 1.0f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Quad:
		{
			auto cube = world.CreateAGameObject<pimm::Quad>();
			cube->GetTransform().SetScale({ 1.0f });
			cube->GetTransform().SetPosition(spawnPosition);
			cube->GetMaterialComponent().SetMaterial(m_woodMaterial);
			if (withPhysics)
			{
				auto* body = cube->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(cube->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}	
	}
}

void MainGame::SaveScene()
{
	wchar_t filePath[MAX_PATH] = {};

	OPENFILENAMEW ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = L"PIMM Scene (*.pimm)\0*.pimm\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"pimm";
	ofn.Flags = OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY;

	if (GetSaveFileNameW(&ofn))
	{
		pimm::SceneSerializer::Save(GetWorld(), filePath);
	}
}

void MainGame::LoadScene()
{
	wchar_t filePath[MAX_PATH] = {};

	OPENFILENAMEW ofn = {};
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = nullptr;
	ofn.lpstrFilter = L"PIMM Scene (*.pimm)\0*.pimm\0";
	ofn.lpstrFile = filePath;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrDefExt = L"pimm";
	ofn.Flags = OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;

	if (GetOpenFileNameW(&ofn))
	{
		pimm::SceneSerializer::Load(GetWorld(), filePath, GetResourceManager());

		auto& world = GetWorld();
		auto player = world.CreateAGameObject<Player>();
		player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });
		player->GetTransform().SetRotation({ 20.0f, 0.0f, 0.0f });
	}
}