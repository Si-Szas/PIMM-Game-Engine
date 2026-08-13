#pragma once
#include <PIMM/Core/Core.h>
#include <string>

namespace pimm
{
	class World;
	class ResourceManager;

	class SceneSerializer
	{
		pimm_disable_copy_and_move(SceneSerializer)

	public:
		SceneSerializer() = default;

		static bool Save(const World& world, const std::wstring& path);
		static bool Load(World& world, const std::wstring& path, ResourceManager& resourceManager);
	};
}
