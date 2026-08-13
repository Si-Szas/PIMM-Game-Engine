#pragma once
#include <string>

namespace pimm
{
	namespace SceneKey
	{
		constexpr const char* Scene = "Scene";
		constexpr const char* Objects = "objects";
		constexpr const char* Object = "Object";
		constexpr const char* Material = "Material";
		constexpr const char* RigidBody = "RigidBody";
		constexpr const char* Camera = "Camera";
		constexpr const char* Controller = "Controller";
		constexpr const char* MeshComponent = "MeshComponent";
		constexpr const char* Children = "Children";
		constexpr const char* State = "State";

		constexpr const char* Name = "name";
		constexpr const char* Enabled = "enabled";
		constexpr const char* Version = "version";
		constexpr const char* Type = "type";
		constexpr const char* Parent = "parent";
		constexpr const char* Position = "position";
		constexpr const char* Scale = "scale";
		constexpr const char* Rotation = "rotation";

		constexpr const char* Shader = "shader";
		constexpr const char* Color = "color";

		constexpr const char* BodyType = "bodyType";
		constexpr const char* Mass = "mass";

		constexpr const char* NearPlane = "nearPlane";
		constexpr const char* FarPlane = "farPlane";
		constexpr const char* Fov = "fov";
		constexpr const char* Active = "active";

		constexpr const char* Sensitivity = "sensitivity";
		constexpr const char* Speed = "speed";

		constexpr const char* Mesh = "mesh";

		constexpr const char* TexturePrefix = "texture";
		constexpr const char* ColliderPrefix = "collider";
		constexpr const char* MaterialPrefix = "material";

		constexpr const char* ColliderBox = "box";
		constexpr const char* ColliderSphere = "sphere";
		constexpr const char* ColliderCapsule = "capsule";

		constexpr const char* HalfExtents = "halfExtents";
		constexpr const char* Radius = "radius";
		constexpr const char* Height = "height";

		constexpr const char* BodyStatic = "Static";
		constexpr const char* BodyKinematic = "Kinematic";
		constexpr const char* BodyDynamic = "Dynamic";

		constexpr const char* FileExtension = "json";
		constexpr const char* FileFilter = "JSON Scene (*.json)\0*.json\0";
	}
}
