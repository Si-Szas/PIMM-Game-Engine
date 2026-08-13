#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Math/Vec3.h>
#include <PIMM/AComponent/RigidBodyComponent.h>

#include <optional>
#include <string>
#include <vector>

namespace pimm
{
	struct MaterialData
	{
		std::wstring shader;
		Vec3 color{ 1.0f };
		std::wstring texture0;
	};

	struct RigidBodyData
	{
		BodyType bodyType = BodyType::Dynamic;
		f32 mass = 1.0f;
		std::vector<ColliderInfo> colliders;
	};

	struct CameraData
	{
		f32 nearPlane = 0.01f;
		f32 farPlane = 100.0f;
		f32 fov = 1.5f;
		bool active = false;
	};

	struct ControllerData
	{
		f32 sensitivity = 0.1f;
		f32 speed = 3.0f;
	};

	struct MeshMatSlot
	{
		std::wstring shaderPath;
		Vec3 color{ 1.0f };
		std::wstring texturePath;
	};

	struct MeshComponentData
	{
		std::wstring meshPath;
		std::vector<MeshMatSlot> materials;
	};

	struct ObjectData
	{
		size_t typeId = 0;
		std::string name;
		bool enabled = true;
		Vec3 position{ 0.0f };
		Vec3 scale{ 1.0f };
		Vec3 rotation{ 0.0f };
		int parentIndex = -1;
		std::vector<ui32> children;

		std::optional<MaterialData> material;
		std::optional<RigidBodyData> rigidBody;
		std::optional<CameraData> camera;
		std::optional<ControllerData> controller;
		std::optional<MeshComponentData> meshComponent;
	};

	struct SceneData
	{
		ui32 version = 1;
		std::vector<ObjectData> objects;
	};
}
