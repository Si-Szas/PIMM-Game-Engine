#include "SceneModeManager.h"
#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/RigidBodyComponent.h>

namespace pimm
{
	void SceneModeManager::EnterPlayMode(World& world)
	{
		if (m_currentMode == SceneMode::Play)
			return;

		SnapshotAllTransforms(world);

		auto objects = world.GetAllGameObjects();
		for (auto* obj : objects)
		{
			if (!obj) continue;
			auto* rb = obj->GetComponent<RigidBodyComponent>();
			if (rb)
			{
				rb->SyncPhysicsFromTransform();
				rb->SetLinearVelocity({ 0.0f });
				rb->SetAngularVelocity({ 0.0f });
			}
		}

		world.SetPhysicsEnabled(true);
		world.ResetPhysicsAccumulator();
		m_currentMode = SceneMode::Play;
	}

	void SceneModeManager::EnterPauseMode(World& world)
	{
		if (m_currentMode != SceneMode::Play)
			return;

		world.SetPhysicsEnabled(false);
		m_currentMode = SceneMode::Pause;
	}

	void SceneModeManager::EnterEditMode(World& world)
	{
		if (m_currentMode == SceneMode::Edit)
			return;

		world.SetPhysicsEnabled(false);
		RestoreAllTransforms(world);
		m_snapshots.clear();
		m_currentMode = SceneMode::Edit;
	}

	void SceneModeManager::RequestFrameStep()
	{
		if (m_currentMode == SceneMode::Pause)
			m_frameStepRequested = true;
	}

	bool SceneModeManager::ConsumeFrameStep()
	{
		if (m_frameStepRequested)
		{
			m_frameStepRequested = false;
			return true;
		}
		return false;
	}

	void SceneModeManager::SnapshotAllTransforms(World& world)
	{
		m_snapshots.clear();
		auto objects = world.GetAllGameObjects();
		for (auto* obj : objects)
		{
			if (!obj) continue;
			auto& transform = obj->GetTransform();
			m_snapshots[obj] = TransformSnapshot{
				transform.GetPosition(),
				transform.GetRotation(),
				transform.GetScale()
			};
		}
	}

	void SceneModeManager::RestoreAllTransforms(World& world)
	{
		auto objects = world.GetAllGameObjects();
		for (auto* obj : objects)
		{
			if (!obj) continue;
			auto it = m_snapshots.find(obj);
			if (it == m_snapshots.end()) continue;
			auto& snap = it->second;
			auto& transform = obj->GetTransform();
			transform.SetPosition(snap.position);
			transform.SetRotation(snap.rotation);
			transform.SetScale(snap.scale);
		}
	}
}
