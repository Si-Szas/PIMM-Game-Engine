#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>
#include "../Editor/SceneMode.h"
#include <functional>
namespace pimm
{
	enum class SpawnObjectType
	{
		Cube = 0,
		Sphere,
		Cylinder,
		Capsule,
		Quad,
		Armadillo,
		Bunny,
		Statue,
		Teapot
	};
	class TopbarPanel final : public pimm::APanel
	{
	public:
		using SpawnCallback = std::function<void(SpawnObjectType, bool)>;
		using ModeCallback = std::function<void()>;
		using FrameStepCallback = std::function<void()>;
		using SceneFileCallback = std::function<void()>;

		TopbarPanel(SpawnCallback onSpawn, ModeCallback onPlay, ModeCallback onPause, ModeCallback onStop, FrameStepCallback onFrameStep, SceneFileCallback onSave, SceneFileCallback onLoad)
			: APanel("Topbar"), m_onSpawn(std::move(onSpawn)), m_onPlay(std::move(onPlay)), m_onPause(std::move(onPause)), m_onStop(std::move(onStop)), m_onFrameStep(std::move(onFrameStep)), m_onSave(std::move(onSave)), m_onLoad(std::move(onLoad)) {
		}

		void SetCurrentMode(SceneMode mode) { m_currentMode = mode; }

		void Render() override {
			if (ImGui::BeginMainMenuBar())
			{
				if (ImGui::BeginMenu("GameObject"))
				{
					if (ImGui::MenuItem("Cube") && m_onSpawn) m_onSpawn(SpawnObjectType::Cube, true);
					if (ImGui::MenuItem("Sphere") && m_onSpawn) m_onSpawn(SpawnObjectType::Sphere, true);
					if (ImGui::MenuItem("Cylinder") && m_onSpawn) m_onSpawn(SpawnObjectType::Cylinder, true);
					if (ImGui::MenuItem("Capsule") && m_onSpawn) m_onSpawn(SpawnObjectType::Capsule, true);
					if (ImGui::MenuItem("Quad") && m_onSpawn) m_onSpawn(SpawnObjectType::Quad, true);
					if (ImGui::MenuItem("Armadillo") && m_onSpawn) m_onSpawn(SpawnObjectType::Armadillo, true);
					if (ImGui::MenuItem("Bunny") && m_onSpawn) m_onSpawn(SpawnObjectType::Bunny, true);
					if (ImGui::MenuItem("Statue") && m_onSpawn) m_onSpawn(SpawnObjectType::Statue, true);
					if (ImGui::MenuItem("Teapot") && m_onSpawn) m_onSpawn(SpawnObjectType::Teapot, true);
					ImGui::EndMenu();
				}
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Save Scene") && m_onSave) m_onSave();
				if (ImGui::MenuItem("Load Scene") && m_onLoad) m_onLoad();
				ImGui::EndMenu();
			}
				ImGui::Separator();

				ImGui::BeginDisabled(m_currentMode != SceneMode::Edit);
				if (ImGui::Button("Play"))
					if (m_onPlay) m_onPlay();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(m_currentMode != SceneMode::Play);
				if (ImGui::Button("Pause"))
					if (m_onPause) m_onPause();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(m_currentMode != SceneMode::Pause);
				if (ImGui::Button("Step"))
					if (m_onFrameStep) m_onFrameStep();
				ImGui::EndDisabled();

				ImGui::SameLine();
				ImGui::BeginDisabled(m_currentMode == SceneMode::Edit);
				if (ImGui::Button("Stop"))
					if (m_onStop) m_onStop();
				ImGui::EndDisabled();

				ImGui::EndMainMenuBar();
			}
		}
	private:
		SpawnCallback m_onSpawn;
		ModeCallback m_onPlay;
		ModeCallback m_onPause;
		ModeCallback m_onStop;
		FrameStepCallback m_onFrameStep;
		SceneFileCallback m_onSave;
		SceneFileCallback m_onLoad;
		SceneMode m_currentMode = SceneMode::Edit;
	};
}