#include "MainGame.h"
#include "Player/Player.h"
#include "UI/CreditsPanel.h"
#include "UI/ColorPickerPanel.h"
#include "UI/HierarchyPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/AssetsPanel.h"
#include <PIMM/Graphics/ImGui/imgui.h>
#include <d3d11.h>
#include <wincodec.h>
#include <vector>
#pragma comment(lib, "windowscodecs.lib")
using namespace pimm;

static ID3D11ShaderResourceView* LoadTextureFromFile(ID3D11Device* device, const wchar_t* filename)
{
    HRESULT hr = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
    bool needsUninit = SUCCEEDED(hr);

    IWICImagingFactory* wicFactory = nullptr;
    hr = CoCreateInstance(CLSID_WICImagingFactory, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&wicFactory));
    if (FAILED(hr))
    {
        if (needsUninit) CoUninitialize();
        return nullptr;
    }

    IWICBitmapDecoder* decoder = nullptr;
    hr = wicFactory->CreateDecoderFromFilename(filename, nullptr, GENERIC_READ, WICDecodeMetadataCacheOnLoad, &decoder);
    if (FAILED(hr)) { wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    IWICBitmapFrameDecode* frame = nullptr;
    hr = decoder->GetFrame(0, &frame);
    if (FAILED(hr)) { decoder->Release(); wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    UINT width = 0, height = 0;
    frame->GetSize(&width, &height);

    IWICFormatConverter* converter = nullptr;
    hr = wicFactory->CreateFormatConverter(&converter);
    if (FAILED(hr)) { frame->Release(); decoder->Release(); wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    hr = converter->Initialize(frame, GUID_WICPixelFormat32bppRGBA, WICBitmapDitherTypeNone, nullptr, 0.0, WICBitmapPaletteTypeCustom);
    if (FAILED(hr)) { converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    std::vector<BYTE> pixels(static_cast<size_t>(width) * height * 4);
    hr = converter->CopyPixels(nullptr, width * 4, static_cast<UINT>(pixels.size()), pixels.data());
    if (FAILED(hr)) { converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    D3D11_TEXTURE2D_DESC desc = {};
    desc.Width = width;
    desc.Height = height;
    desc.MipLevels = 1;
    desc.ArraySize = 1;
    desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    desc.SampleDesc.Count = 1;
    desc.Usage = D3D11_USAGE_DEFAULT;
    desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;

    D3D11_SUBRESOURCE_DATA initData = {};
    initData.pSysMem = pixels.data();
    initData.SysMemPitch = width * 4;

    ID3D11Texture2D* texture = nullptr;
    hr = device->CreateTexture2D(&desc, &initData, &texture);
    if (FAILED(hr)) { converter->Release(); frame->Release(); decoder->Release(); wicFactory->Release(); if (needsUninit) CoUninitialize(); return nullptr; }

    ID3D11ShaderResourceView* srv = nullptr;
    hr = device->CreateShaderResourceView(texture, nullptr, &srv);

    texture->Release();
    converter->Release();
    frame->Release();
    decoder->Release();
    wicFactory->Release();
    if (needsUninit) CoUninitialize();

    return SUCCEEDED(hr) ? srv : nullptr;
}

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

	auto creditsPanel = std::make_unique<CreditsPanel>();
	m_creditsPanel = creditsPanel.get();
	uiManager->RegisterPanel(std::move(creditsPanel));

	auto colorPickerPanel = std::make_unique<ColorPickerPanel>();
	m_colorPickerPanel = colorPickerPanel.get();
	uiManager->RegisterPanel(std::move(colorPickerPanel));

	auto& gfxDevice = GetGraphicsDevice();
	ID3D11ShaderResourceView* logoTexture = LoadTextureFromFile(gfxDevice.GetD3DDevice().Get(), L"Game/Assets/Textures/dlsu logo.png");
	if (logoTexture)
		m_creditsPanel->SetLogoTexture(reinterpret_cast<ImTextureID>(logoTexture));
}

void MainGame::OnUpdate(f32 deltaTime)
{
	Game::OnUpdate(deltaTime);
}

void MainGame::OnUIRender()
{
	// Main menu bar
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("About"))
		{
			if (ImGui::MenuItem("Credits"))
				m_creditsPanel->SetShow(true);
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Tools"))
		{
			if (ImGui::MenuItem("Color Picker"))
				m_colorPickerPanel->SetShow(true);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport(), ImGuiDockNodeFlags_PassthruCentralNode);
	auto* uiManager = GetUIManager();
	uiManager->RenderPanels();
}
