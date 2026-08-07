#pragma once
#include <PIMM/Core/Base.h>
#include <PIMM/Core/Core.h>

namespace pimm
{
	class Game
	{
		pimm_disable_copy_and_move(Game)
		public:
			//CONSTRUCTOR
			explicit Game(const GameDescriptor& descriptor);

			//FUNCTIONS
				//final is used to indicate that run cannot be further overwritten or inherited
			virtual void Run() final;
			
			//GETTER
			virtual World& GetWorld() noexcept final;
			virtual Logger& GetLogger() const noexcept final;
			virtual InputSystem& GetInputSystem() noexcept final;
			pimm::UIManager* GetUIManager() noexcept { return m_uiManager.get(); }
			virtual ResourceManager& GetResourceManager() noexcept final;
			virtual WorldRenderer& GetWorldRenderer() noexcept final;

			//DESTRUCTOR
			virtual ~Game();

		protected:
			virtual void OnCreate()
			{
			}

			virtual void OnUpdate(f32 deltaTime) 
			{
			} 
			virtual void OnUIRender()
			{
			}
			
		private:
			void OnInternalUpdate(f32 deltaTime); //Act as a callback where we can place all the code that we need to execute each frame

		private:
			UniquePtr<Logger> m_logger{};
			UniquePtr<InputSystem> m_inputSystem{};
			//Last defined attirbute is first to be allocated, so we want our window to be first
			RefPtr<GraphicsDevice> m_graphicsDevice{};
			//Using smart pointers avoid memory leaks in case a pointer is not deleted
			UniquePtr<Display> m_display{};
			// UI Manager
			UniquePtr<UIManager> m_uiManager{};
			//Unique pointer to resource manager
			UniquePtr<ResourceManager> m_resourceManager{};
			//World Renderer
			UniquePtr<WorldRenderer> m_worldRenderer{};
			//Unique pointer to the world
			UniquePtr<World> m_world{};
			//Game loop is running
			bool m_isRunning{ true };
	};
}

