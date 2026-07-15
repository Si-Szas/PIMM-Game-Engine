#include <PIMM/Game/Game.h>
#include <PIMM/Window/Window.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Core/Logger.h>
#include <PIMM/Game/Display.h>
#include <PIMM/Game/World.h>
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/InputSystem/InputSystem.h>
#include <PIMM/Graphics/UIManager/UIManager.h>
#include <PIMM/Resource/ResourceManager.h>

pimm::Game::Game(const GameDescriptor& descriptor)
{
	m_logger = std::make_unique<Logger>(descriptor.logLevel);
	PIMMLogInformation("| PIMM | DirectX C++ Game Engine |");
	PIMMLogInformation("|--------------------------------|\n");
	//Since input system needs world and uiManager and neither exist yet, pass null for now then initialize later
	m_inputSystem = std::make_unique<InputSystem>(InputSystemDescriptor{ { *m_logger }, nullptr, nullptr });
	m_graphicsDevice = std::make_unique<GraphicsDevice>(GraphicsDeviceDescriptor{ *m_logger });
	m_display = std::make_unique<Display>(DisplayDescriptor{ {*m_logger, descriptor.windowSize}, *m_graphicsDevice });
	m_uiManager = std::make_unique<UIManager>(UIManagerDescriptor{ {*m_logger}, *m_graphicsDevice, m_display->GetHandle() });

	//Initialize the resource manager
	m_resourceManager = std::make_unique<ResourceManager>(ResourceManagerDescriptor{ {*m_logger}, SystemContext{ *m_graphicsDevice } });
	
	//Initialize the world renderer
	m_worldRenderer = std::make_unique<WorldRenderer>(WorldRendererDescriptor{ {*m_logger}, *m_graphicsDevice, *m_uiManager });
	//Initialize world
	m_world = std::make_unique<World>(WorldDescriptor{ BaseDescriptor{*m_logger}, GameContext{*m_inputSystem, *m_resourceManager}, {*m_worldRenderer} });
	//Set world in input system to be initialized since it was passed as null at first
	m_inputSystem->SetWorld(*m_world);
	m_inputSystem->SetUIManager(*m_uiManager);

	//TEMPORARY CURSOR LOCK
	m_inputSystem->SetCursorLockArea(m_display->GetClientAreaInScreenSpace());
	PIMMLogInformation("Game successfully initialized.");
}
pimm::Game::~Game()
{
	PIMMLogInformation("Game is shutting down...");
}
void pimm::Game::OnInternalUpdate(f32 deltaTime)
{
	m_inputSystem->Update();
	m_uiManager->NewFrame();
	OnUpdate(deltaTime);
	OnUIRender();
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

pimm::ResourceManager& pimm::Game::GetResourceManager() noexcept
{
	return *m_resourceManager;
}

pimm::World& pimm::Game::GetWorld() noexcept
{
	return *m_world;
}