#pragma once
#include <PIMM/All.h>
#include "Editor/SceneModeManager.h"
#include <vector>

namespace pimm { class TopbarPanel; }

class MainGame : public pimm::Game
{
	public:
		explicit MainGame(const pimm::GameDescriptor& descriptor);
	protected:
		virtual void OnCreate();
		virtual void OnUpdate(pimm::f32 deltaTime);
		void OnUIRender() override;
	private:
		void SpawnObject(pimm::SpawnObjectType type, bool withPhysics);
		void SaveScene();
		void LoadScene();
	private:
		pimm::SceneModeManager m_sceneModeManager{};
		pimm::TopbarPanel* m_topbarPanel = nullptr;
		std::shared_ptr<pimm::MaterialResource> m_woodMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_stoneMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_brickMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_basicMaterial{};
		std::shared_ptr<pimm::MaterialResource> m_purpleMaterial{};
};