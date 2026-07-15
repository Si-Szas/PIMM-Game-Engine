#include <PIMM/Game/Game.h>
#include <PIMM/Window/Window.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Core/Logger.h>
#include <PIMM/Game/Display.h>
#include <PIMM/Game/World.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/InputSystem/InputSystem.h>

pimm::Game::Game(const GameDescriptor& descriptor)
{
	m_logger = std::make_unique<Logger>(descriptor.logLevel);

	PIMMLogInformation("| PIMM | DirectX C++ Game Engine |");
	PIMMLogInformation("|--------------------------------|\n");

	//Since input system needs world and world needs input system, we just set it as null for now then initialize later
	m_inputSystem = std::make_unique<InputSystem>(InputSystemDescriptor{ { *m_logger }, {nullptr} });
	m_graphicsDevice = std::make_unique<GraphicsDevice>(GraphicsDeviceDescriptor{ *m_logger });
	m_display = std::make_unique<Display>(DisplayDescriptor{ {*m_logger, descriptor.windowSize}, *m_graphicsDevice });
	m_worldRenderer = std::make_unique<WorldRenderer>(WorldRendererDescriptor{ {*m_logger}, *m_graphicsDevice });
	//Initialize world
	m_world = std::make_unique<World>(WorldDescriptor{ BaseDescriptor{*m_logger}, GameContext{*m_inputSystem}, {*m_worldRenderer} });
	//Set world in input system to be initialized since it was passed as null at first
	m_inputSystem->SetWorld(*m_world);

	//TEMPORARY CURSOR LOCK
	m_inputSystem->SetCursorLockArea(m_display->GetClientAreaInScreenSpace());

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(static_cast<HWND>(m_display->GetHandle()));
	ImGui_ImplDX11_Init(
		m_graphicsDevice.get()->GetD3DDevice().Get(), 
		m_graphicsDevice.get()->GetD3DDeviceContext().Get()
	);
	ImGui::StyleColorsDark();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows

	PIMMLogInformation("Game successfully initialized.");
}

pimm::Game::~Game()
{
	//Ensure proper clean up of IMGUI
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}

	PIMMLogInformation("Game is shutting down...");
}

void pimm::Game::OnInternalUpdate(f32 deltaTime)
{
	m_inputSystem->Update();

	OnUpdate(deltaTime);
	m_world->Update(deltaTime);
	m_worldRenderer->Render(*m_world, m_display->GetSwapChain(), deltaTime);
}

pimm::Logger& pimm::Game::GetLogger() noexcept
{
	return *m_logger;
}

pimm::InputSystem& pimm::Game::GetInputSystem() noexcept
{
	return *m_inputSystem;
}

pimm::World& pimm::Game::GetWorld() noexcept
{
	return *m_world;
}
