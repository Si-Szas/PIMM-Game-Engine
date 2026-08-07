#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>
#include <functional>
namespace pimm
{
	enum class SpawnObjectType
	{
		Cube = 0,
		Sphere,
		Cylinder,
		Capsule
	};
	class TopbarPanel final : public pimm::APanel
	{
	public:
		using SpawnCallback = std::function<void(SpawnObjectType, bool)>;
		TopbarPanel(SpawnCallback onSpawn)
			: APanel("Topbar"), m_onSpawn(std::move(onSpawn)) {
		}
		void Render() override {
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("GameObject"))
				{
					ImGui::Checkbox("With Physics", &m_spawnWithPhysics);
					ImGui::Separator();
					if (ImGui::MenuItem("Cube") && m_onSpawn) m_onSpawn(SpawnObjectType::Cube, m_spawnWithPhysics);
					if (ImGui::MenuItem("Sphere") && m_onSpawn) m_onSpawn(SpawnObjectType::Sphere, m_spawnWithPhysics);
					if (ImGui::MenuItem("Cylinder") && m_onSpawn) m_onSpawn(SpawnObjectType::Cylinder, m_spawnWithPhysics);
					if (ImGui::MenuItem("Capsule") && m_onSpawn) m_onSpawn(SpawnObjectType::Capsule, m_spawnWithPhysics);
					ImGui::EndMenu();
				}
				ImGui::EndMainMenuBar();
			}
		}
	private:
		SpawnCallback m_onSpawn;
		bool m_spawnWithPhysics = true;
	};
}