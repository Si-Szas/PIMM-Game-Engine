#include "SceneSerializer.h"
#include "SceneJsonConverter.h"
#include <PIMM/Game/World.h>
#include <PIMM/Resource/ResourceManager.h>

#include <fstream>

namespace pimm
{
	bool SceneSerializer::Save(const World& world, const std::wstring& path)
	{
		nlohmann::json document = ToJson(WorldToSceneData(world));

		std::ofstream file(path);
		if (!file.is_open())
			return false;

		file << document.dump(4);

		return true;
	}

	bool SceneSerializer::Load(World& world, const std::wstring& path, ResourceManager& resourceManager)
	{
		std::ifstream file(path);
		if (!file.is_open())
			return false;

		try
		{
			nlohmann::json document = nlohmann::json::parse(file);
			SceneDataToWorld(world, FromJson(document), resourceManager);
		}
		catch (...)
		{
			return false;
		}

		return true;
	}
}
