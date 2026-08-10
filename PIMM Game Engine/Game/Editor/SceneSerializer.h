#pragma once
#include <PIMM/Core/Core.h>
#include <string>
#include <unordered_map>

namespace pimm
{
	class World;
	class ResourceManager;
	class AGameObject;
	enum class SceneMode;

	class SceneSerializer
	{
		pimm_disable_copy_and_move(SceneSerializer)

	public:
		SceneSerializer() = default;

		static bool Save(const World& world, const std::wstring& path);
		static bool Load(World& world, const std::wstring& path, ResourceManager& resourceManager);

		static std::unordered_map<size_t, std::unordered_map<SceneMode, struct TransformSnapshot>> ParseStates(const std::wstring& path);

	private:
		static std::string GetTypeName(size_t typeId);
		static size_t GetTypeIdFromName(const std::string& name);
	};
}
