#pragma once
#include <PIMM/All.h>
#include <vector>

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
		void SpawnObject(pimm::SpawnObjectType type, bool withPhysics);
	private:
		//LIST OF GAME OBJECTS
		//std::vector<pimm::AGameObject*> m_objectList{};
		//pimm::AGameObject* m_player{};
		//pimm::f32 m_rotation{};
		//MATERIALS
		std::shared_ptr<pimm::MaterialResource> m_woodMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_stoneMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_basicMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_purpleMaterial{};
};