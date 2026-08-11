#pragma once
#include "SceneMode.h"
#include <PIMM/Core/Core.h>
#include <unordered_map>

namespace pimm
{
	class World;
	class AGameObject;

	class SceneModeManager
	{
		pimm_disable_copy_and_move(SceneModeManager)

	public:
		SceneModeManager() = default;

		SceneMode GetMode() const noexcept { return m_currentMode; }
		bool IsEditMode() const noexcept { return m_currentMode == SceneMode::Edit; }
		bool IsPlayMode() const noexcept { return m_currentMode == SceneMode::Play; }
		bool IsPauseMode() const noexcept { return m_currentMode == SceneMode::Pause; }

		void EnterPlayMode(World& world);
		void EnterPauseMode(World& world);
		void EnterEditMode(World& world);

		void RequestFrameStep();
		bool ConsumeFrameStep();

	private:
		void SnapshotAllTransforms(World& world);
		void RestoreAllTransforms(World& world);

		SceneMode m_currentMode = SceneMode::Edit;
		std::unordered_map<AGameObject*, TransformSnapshot> m_snapshots;
		bool m_frameStepRequested = false;
	};
}
