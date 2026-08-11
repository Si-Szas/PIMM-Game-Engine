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

	m_basicThreePointMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicThreePoint.hlsl");
	if (m_basicThreePointMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_basicThreePointMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	m_basicFourPointMaterial = GetResourceManager().CreateResourceFromFile<pimm::MaterialResource>(L"Game/Assets/Shaders/BasicFourPoint.hlsl");
	if (m_basicFourPointMaterial)
	{
		auto materialData = pimm::Vec3(1.0f);
		m_basicFourPointMaterial->SetData(std::as_bytes(std::span{ &materialData, 1 }));
	}

	m_armadilloMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/armadillo.obj");
	m_bunnyMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/bunny.obj");
	m_statueMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/statue.obj");
	m_teapotMesh = GetResourceManager().CreateResourceFromFile<pimm::MeshResource>(L"Game/Assets/Meshes/teapot.obj");

	auto player = world.CreateAGameObject<Player>();
	player->GetTransform().SetPosition({ 0.0f, 1.0f, -3.0f });
	player->GetTransform().SetRotation({ 20.0f, 0.0f, 0.0f });

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
			PIMMLogInformation("Spawned Cube. With Physics is {}", withPhysics);

			auto cube = world.CreateAGameObject<pimm::Cube>();
			cube->GetTransform().SetScale({ 1.0f });
			cube->GetTransform().SetPosition(spawnPosition);
			cube->GetMaterialComponent().SetMaterial(m_basicFourPointMaterial);
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
			PIMMLogInformation("Spawned Sphere. With Physics is {}", withPhysics);

			auto sphere = world.CreateAGameObject<pimm::Sphere>();
			sphere->GetTransform().SetScale({ 1.0f });
			sphere->GetTransform().SetPosition(spawnPosition);
			sphere->GetMaterialComponent().SetMaterial(m_basicFourPointMaterial);
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
			PIMMLogInformation("Spawned Cylinder. With Physics is {}", withPhysics);

			auto cylinder = world.CreateAGameObject<pimm::Cylinder>();
			cylinder->GetTransform().SetScale({ 1.0f });
			cylinder->GetTransform().SetPosition(spawnPosition);
			cylinder->GetMaterialComponent().SetMaterial(m_basicFourPointMaterial);
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
			PIMMLogInformation("Spawned Capsule. With Physics is {}", withPhysics);

			auto capsule = world.CreateAGameObject<pimm::Capsule>();
			capsule->GetTransform().SetScale({ 1.0f });
			capsule->GetTransform().SetPosition(spawnPosition);
			capsule->GetMaterialComponent().SetMaterial(m_basicFourPointMaterial);
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
			PIMMLogInformation("Spawned Quad. With Physics is {}", withPhysics);

			auto cube = world.CreateAGameObject<pimm::Quad>();
			cube->GetTransform().SetScale({ 1.0f });
			cube->GetTransform().SetPosition(spawnPosition);
			cube->GetMaterialComponent().SetMaterial(m_basicFourPointMaterial);
			if (withPhysics)
			{
				auto* body = cube->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(cube->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}	
		case pimm::SpawnObjectType::Armadillo:
		{
			PIMMLogInformation("Spawned Armadillo. With Physics is {}", withPhysics);

			auto mesh = world.CreateAGameObject<pimm::MeshObject>();
			auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();

			comp->SetMesh(m_armadilloMesh);
			comp->SetMaterial(0, m_basicThreePointMaterial);
			mesh->GetTransform().SetScale({ 1.0f });
			mesh->GetTransform().SetPosition(spawnPosition);
			mesh->GetTransform().SetRotation({ 0.0f, -180.0f, 0.0f });
			mesh->SetObjectName("Armadillo");

			if (withPhysics)
			{
				auto* body = mesh->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(mesh->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Bunny:
		{
			PIMMLogInformation("Spawned Bunny. With Physics is {}", withPhysics);

			auto mesh = world.CreateAGameObject<pimm::MeshObject>();
			auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();

			comp->SetMesh(m_bunnyMesh);
			comp->SetMaterial(0, m_basicThreePointMaterial);
			mesh->GetTransform().SetScale({ 1.0f });
			mesh->GetTransform().SetPosition(spawnPosition);
			mesh->SetObjectName("Bunny");

			if (withPhysics)
			{
				auto* body = mesh->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(mesh->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Statue:
		{
			PIMMLogInformation("Spawned Statue. With Physics is {}", withPhysics);

			auto mesh = world.CreateAGameObject<pimm::MeshObject>();
			auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();

			comp->SetMesh(m_statueMesh);
			comp->SetMaterial(0, m_basicThreePointMaterial);
			mesh->GetTransform().SetScale({ 1.0f });
			mesh->GetTransform().SetPosition(spawnPosition);
			mesh->SetObjectName("Statue");

			if (withPhysics)
			{
				auto* body = mesh->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(mesh->GetTransform().GetScale() * 0.5f);
				body->SetMass(1.0f);
			}
			break;
		}
		case pimm::SpawnObjectType::Teapot:
		{
			PIMMLogInformation("Spawned Teapot. With Physics is {}", withPhysics);

			auto mesh = world.CreateAGameObject<pimm::MeshObject>();
			auto comp = mesh->CreateOrGetComponent<pimm::MeshComponent>();

			comp->SetMesh(m_teapotMesh);
			comp->SetMaterial(0, m_basicThreePointMaterial);
			mesh->GetTransform().SetScale({ 1.0f });
			mesh->GetTransform().SetPosition(spawnPosition);
			mesh->SetObjectName("Teapot");

			if (withPhysics)
			{
				auto* body = mesh->CreateOrGetComponent<pimm::RigidBodyComponent>();
				body->AddBoxCollider(mesh->GetTransform().GetScale() * 0.5f);
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