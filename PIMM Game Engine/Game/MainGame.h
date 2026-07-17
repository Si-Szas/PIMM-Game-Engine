#pragma once
#include <PIMM/All.h>
#include <vector>

// Forward declarations for UI panels
class CreditsPanel;
class ColorPickerPanel;

//MainGame will mainly hold the game objects (player, camera, etc.) that are present in the game
class MainGame : public pimm::Game
{
	public:
		//CONSTRUCTOR
		explicit MainGame(const pimm::GameDescriptor& descriptor);

	protected:
		virtual void OnCreate();
		virtual void OnUpdate(pimm::f32 deltaTime);
		void OnUIRender() override;
	private:
		//LIST OF GAME OBJECTS
		//std::vector<pimm::AGameObject*> m_objectList{};
		//pimm::AGameObject* m_player{};
		//pimm::f32 m_rotation{};

		// Panel raw pointers (owned by UIManager)
		CreditsPanel* m_creditsPanel = nullptr;
		ColorPickerPanel* m_colorPickerPanel = nullptr;
};

