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
		void undoAction(pimm::World& world);
	private:
		pimm::SceneModeManager m_sceneModeManager{};
		pimm::TopbarPanel* m_topbarPanel = nullptr;
		pimm::RefPtr<pimm::MaterialResource> m_basicThreePointMaterial{};
		pimm::RefPtr<pimm::MaterialResource> m_basicFourPointMaterial{};

		pimm::AGameObject::Snapshot* snapshot = nullptr;

		pimm::RefPtr<pimm::MeshResource> m_armadilloMesh;
		pimm::RefPtr<pimm::MeshResource> m_bunnyMesh;
		pimm::RefPtr<pimm::MeshResource> m_statueMesh;
		pimm::RefPtr<pimm::MeshResource> m_teapotMesh;
};