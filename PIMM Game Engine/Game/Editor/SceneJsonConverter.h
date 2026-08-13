#pragma once
#include "SceneData.h"

#include <nlohmann/json.hpp>

namespace pimm
{
	class World;
	class ResourceManager;

	SceneData WorldToSceneData(const World& world);
	void SceneDataToWorld(World& world, const SceneData& data, ResourceManager& resourceManager);

	nlohmann::json ToJson(const SceneData& data);
	SceneData FromJson(const nlohmann::json& document);
}
