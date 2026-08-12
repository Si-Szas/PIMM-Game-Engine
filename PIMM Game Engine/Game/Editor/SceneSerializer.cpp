#include "SceneSerializer.h"
#include "SceneFormat.h"
#include "SceneMode.h"
#include "../Player/Player.h"
#include <PIMM/Game/World.h>
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AGameObject/Quad.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/AGameObject/Capsule.h>
#include <PIMM/AGameObject/MeshObject.h>
#include <PIMM/AGameObject/CameraObject.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>
#include <PIMM/AComponent/RigidBodyComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AComponent/ControllerComponent.h>
#include <PIMM/AComponent/MeshComponent.h>
#include <PIMM/Resource/ResourceManager.h>
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Resource/TextureResource.h>
#include <PIMM/Resource/MeshResource.h>

#include <fstream>
#include <sstream>
#include <vector>
#include <unordered_map>
#include <string>

namespace pimm
{
	namespace
	{
		std::string Narrow(const std::wstring& w)
		{
			return std::string(w.begin(), w.end());
		}

		void WriteVec3(std::ofstream& f, const Vec3& v)
		{
			f << v.x << ' ' << v.y << ' ' << v.z << '\n';
		}

		bool ParseVec3(const std::string& line, Vec3& out)
		{
			std::istringstream iss(line);
			return static_cast<bool>(iss >> out.x >> out.y >> out.z);
		}

		std::string BodyTypeToString(BodyType type)
		{
			switch (type)
			{
			case BodyType::Static:    return SceneKey::BodyStatic;
			case BodyType::Kinematic: return SceneKey::BodyKinematic;
			default:                  return SceneKey::BodyDynamic;
			}
		}

		BodyType StringToBodyType(const std::string& s)
		{
			if (s == SceneKey::BodyStatic)    return BodyType::Static;
			if (s == SceneKey::BodyKinematic) return BodyType::Kinematic;
			return BodyType::Dynamic;
		}

		std::string ColliderTypeToString(ColliderType type)
		{
			switch (type)
			{
			case ColliderType::Box:     return SceneKey::ColliderBox;
			case ColliderType::Sphere:  return SceneKey::ColliderSphere;
			default:                    return SceneKey::ColliderCapsule;
			}
		}

		void WriteTransform(std::ofstream& f, TransformComponent& t)
		{
			f << SceneKey::Position << " = ";
			WriteVec3(f, t.GetPosition());
			f << SceneKey::Scale << " = ";
			WriteVec3(f, t.GetScale());
			f << SceneKey::Rotation << " = ";
			WriteVec3(f, t.GetRotation());
		}

		void WriteRigidBody(std::ofstream& f, RigidBodyComponent& rb)
	{
		f << SceneKey::BodyType << " = \"" << BodyTypeToString(rb.GetBodyType()) << "\"\n";
		f << SceneKey::Mass << " = " << rb.GetMass() << '\n';

		for (ui32 i = 0; i < rb.GetColliderCount(); ++i)
		{
			const auto& c = rb.GetCollider(i);
			f << SceneKey::ColliderPrefix << i << " = ";
			f << ColliderTypeToString(c.type) << ' ';
			switch (c.type)
			{
			case ColliderType::Box:     WriteVec3(f, c.halfExtents); break;
			case ColliderType::Sphere:  f << c.radius << '\n'; break;
			case ColliderType::Capsule: f << c.radius << ' ' << c.height << '\n'; break;
			}
		}
	}

		void WriteMaterial(std::ofstream& f, MaterialComponent& mc)
		{
			auto* mat = mc.GetMaterial();
			if (!mat) return;

			f << SceneKey::Shader << " = \"" << Narrow(mat->GetPath()) << "\"\n";

			auto data = mat->GetData();
			if (data.size() >= sizeof(Vec3))
			{
				const Vec3* color = reinterpret_cast<const Vec3*>(data.data());
				f << SceneKey::Color << " = ";
				WriteVec3(f, *color);
			}

			auto* tex = mat->GetTexture(0);
			if (tex)
				f << SceneKey::TexturePrefix << "0 = \"" << Narrow(tex->GetPath()) << "\"\n";
		}

		void WriteCamera(std::ofstream& f, CameraComponent& cam)
		{
			f << SceneKey::NearPlane << " = " << cam.GetNearPlane() << '\n';
			f << SceneKey::FarPlane << " = " << cam.GetFarPlane() << '\n';
			f << SceneKey::Fov << " = " << cam.GetFieldOfView() << '\n';
			f << SceneKey::Active << " = " << (cam.IsActive() ? 1 : 0) << '\n';
		}

		void WriteController(std::ofstream& f, ControllerComponent& ctrl)
		{
			f << SceneKey::Sensitivity << " = " << ctrl.GetSensitivity() << '\n';
			f << SceneKey::Speed << " = " << ctrl.GetSpeedModifier() << '\n';
		}

		void WriteMeshComponent(std::ofstream& f, MeshComponent& mc)
	{
		auto* mesh = mc.GetMesh();
		if (mesh)
			f << SceneKey::Mesh << " = \"" << Narrow(mesh->GetPath()) << "\"\n";

		ui32 materialCount = mc.GetMaterialCount();

		for (ui32 i = 0; i < materialCount; ++i)
		{
			auto* mat = mc.GetMaterial(i);
			if (!mat) continue;

			f << SceneKey::MaterialPrefix << i << " = \"" << Narrow(mat->GetPath()) << "\"";

			auto data = mat->GetData();
			if (data.size() >= sizeof(Vec3))
			{
				const Vec3* color = reinterpret_cast<const Vec3*>(data.data());
				f << ' ' << color->x << ' ' << color->y << ' ' << color->z;
			}

			auto* tex = mat->GetTexture(0);
			if (tex)
				f << " \"" << Narrow(tex->GetPath()) << '\"';

			f << '\n';
		}
	}

	void WidenPath(std::wstring& out, const std::string& narrow)
	{
		out.assign(narrow.begin(), narrow.end());
	}

		bool ParseQuotedPath(const std::string& value, std::wstring& out)
		{
			size_t firstQuote = value.find('"');
			if (firstQuote == std::string::npos) return false;

			size_t secondQuote = value.find('"', firstQuote + 1);
			if (secondQuote == std::string::npos) return false;

			WidenPath(out, value.substr(firstQuote + 1, secondQuote - firstQuote - 1));
			return true;
		}

}

	std::string SceneSerializer::GetTypeName(size_t typeId)
	{
		static const std::unordered_map<size_t, std::string> map = {
			{ Quad::getTypeId(),      "Quad" },
			{ Cube::getTypeId(),      "Cube" },
			{ Sphere::getTypeId(),    "Sphere" },
			{ Cylinder::getTypeId(),  "Cylinder" },
			{ Capsule::getTypeId(),   "Capsule" },
			{ MeshObject::getTypeId(),"MeshObject" },
			{ CameraObject::getTypeId(),"CameraObject" },
			{ Player::getTypeId(),      "Player" },
		};

		auto it = map.find(typeId);
		return (it != map.end()) ? it->second : "Unknown";
	}

	size_t SceneSerializer::GetTypeIdFromName(const std::string& name)
	{
		static const std::unordered_map<std::string, size_t> map = {
			{ "Quad",       Quad::getTypeId()      },
			{ "Cube",       Cube::getTypeId()      },
			{ "Sphere",     Sphere::getTypeId()    },
			{ "Cylinder",   Cylinder::getTypeId()  },
			{ "Capsule",    Capsule::getTypeId()   },
			{ "MeshObject", MeshObject::getTypeId()},
			{ "CameraObject", CameraObject::getTypeId()},
			{ "Player",       Player::getTypeId()      },
		};

		auto it = map.find(name);
		return (it != map.end()) ? it->second : 0;
	}

	bool SceneSerializer::Save(const World& world, const std::wstring& path)
	{
		std::ofstream file(path);
		if (!file.is_open())
			return false;

		file << "[" << SceneKey::Scene << "]\n";
		file << SceneKey::Version << " = 1\n\n";

		auto objects = world.GetAllGameObjects();

		for (ui32 i = 0; i < objects.size(); ++i)
		{
			auto* obj = objects[i];
			if (!obj) continue;
			if (obj->GetTypeID() == 0) continue;

			file << "[" << SceneKey::Object << ' ' << i << "]\n";
			file << SceneKey::Type << " = \"" << GetTypeName(obj->GetTypeID()) << "\"\n";
			file << SceneKey::Name << " = \"" << obj->GetObjectName() << "\"\n";
			file << SceneKey::Enabled << " = " << (obj->IsEnabled() ? 1 : 0) << '\n';

			auto* parent = obj->GetParent();
			int parentIndex = -1;
			if (parent)
			{
				for (ui32 p = 0; p < objects.size(); ++p)
				{
					if (objects[p] == parent)
					{
						parentIndex = static_cast<int>(p);
						break;
					}
				}
			}
			file << SceneKey::Parent << " = " << parentIndex << '\n';

			WriteTransform(file, obj->GetTransform());

			const auto& children = obj->GetChildren();
			if (!children.empty())
			{
				file << SceneKey::Children << " =";
				for (auto* child : children)
				{
					for (ui32 c = 0; c < objects.size(); ++c)
					{
						if (objects[c] == child)
						{
							file << ' ' << c;
							break;
						}
					}
				}
				file << '\n';
			}

			const auto& components = obj->GetAllComponents();
			for (const auto& [compId, comp] : components)
			{
				if (!comp) continue;

				if (compId == MaterialComponent::getTypeId())
				{
					file << "\n[" << SceneKey::Object << ' ' << i << '.' << SceneKey::Material << "]\n";
					WriteMaterial(file, *static_cast<MaterialComponent*>(comp.get()));
				}
				else if (compId == RigidBodyComponent::getTypeId())
				{
					file << "\n[" << SceneKey::Object << ' ' << i << '.' << SceneKey::RigidBody << "]\n";
					WriteRigidBody(file, *static_cast<RigidBodyComponent*>(comp.get()));
				}
				else if (compId == CameraComponent::getTypeId())
				{
					file << "\n[" << SceneKey::Object << ' ' << i << '.' << SceneKey::Camera << "]\n";
					WriteCamera(file, *static_cast<CameraComponent*>(comp.get()));
				}
				else if (compId == ControllerComponent::getTypeId())
				{
					file << "\n[" << SceneKey::Object << ' ' << i << '.' << SceneKey::Controller << "]\n";
					WriteController(file, *static_cast<ControllerComponent*>(comp.get()));
				}
				else if (compId == MeshComponent::getTypeId())
				{
					file << "\n[" << SceneKey::Object << ' ' << i << '.' << SceneKey::MeshComponent << "]\n";
					WriteMeshComponent(file, *static_cast<MeshComponent*>(comp.get()));
				}
			}

			file << '\n';
		}

		return true;
	}

	bool SceneSerializer::Load(World& world, const std::wstring& path, ResourceManager& resourceManager)
	{
		std::ifstream file(path);
		if (!file.is_open())
			return false;

		struct ParsedObject
		{
			size_t typeId = 0;
			std::string name;
			bool enabled = true;
			Vec3 position{ 0.0f };
			Vec3 scale{ 1.0f };
			Vec3 rotation{ 0.0f };
			int parentIndex = -1;

			bool hasMaterial = false;
			std::wstring materialShader;
			Vec3 materialColor{ 1.0f };
			std::wstring texture0;

			bool hasRigidBody = false;
			BodyType bodyType = BodyType::Dynamic;
			f32 mass = 1.0f;
			std::vector<ColliderInfo> colliders;

			bool hasCamera = false;
			f32 nearPlane = 0.01f;
			f32 farPlane = 100.0f;
			f32 fov = 1.5f;
			bool activeCamera = false;

			bool hasController = false;
			f32 sensitivity = 0.1f;
			f32 speedModifier = 3.0f;

			bool hasMeshComponent = false;
			std::wstring meshPath;
			struct MeshMatSlot
			{
				std::wstring shaderPath;
				Vec3 color{ 1.0f };
				std::wstring texturePath;
			};
			std::vector<MeshMatSlot> meshMaterials;

			std::vector<ui32> childIndices;
		};

		world.DeleteAllAGameObjects();

		std::vector<ParsedObject> parsedObjects;

		std::string line;
		int currentObjectIndex = -1;
		std::string currentSection;

		auto ParseSection = [](const std::string& line) -> std::string
		{
			if (line.size() < 3 || line[0] != '[' || line.back() != ']')
				return {};
			return line.substr(1, line.size() - 2);
		};

		while (std::getline(file, line))
		{
			if (line.empty() || line[0] == '#')
				continue;

			if (line[0] == '[')
			{
				std::string section = ParseSection(line);
				if (section.starts_with(SceneKey::Scene))
				{
					currentObjectIndex = -1;
					currentSection = SceneKey::Scene;
				}
				else if (section.starts_with(SceneKey::Object))
				{
					auto dotPos = section.find('.');
					std::string indexStr;

					if (dotPos == std::string::npos)
					{
						indexStr = section.substr(std::string(SceneKey::Object).length() + 1);
						currentObjectIndex = std::stoi(indexStr);

						while (static_cast<int>(parsedObjects.size()) <= currentObjectIndex)
							parsedObjects.emplace_back();

						currentSection = SceneKey::Object;
					}
					else
					{
						std::string objPart = section.substr(std::string(SceneKey::Object).length() + 1, dotPos - std::string(SceneKey::Object).length() - 1);
						currentObjectIndex = std::stoi(objPart);
						currentSection = section.substr(dotPos + 1);
					}
				}
				continue;
			}

			if (currentObjectIndex < 0)
				continue;

			auto eqPos = line.find('=');
			if (eqPos == std::string::npos)
				continue;

			std::string key = line.substr(0, eqPos);
			std::string value = line.substr(eqPos + 1);

			while (!key.empty() && key.back() == ' ')
				key.pop_back();
			while (!key.empty() && key.front() == ' ')
				key.erase(0, 1);
			while (!value.empty() && value.back() == ' ')
				value.pop_back();
			while (!value.empty() && value.front() == ' ')
				value.erase(0, 1);

			auto& obj = parsedObjects[currentObjectIndex];

			if (currentSection == SceneKey::Object)
			{
				if (key == SceneKey::Type)
				{
					value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
					obj.typeId = GetTypeIdFromName(value);
				}
				else if (key == SceneKey::Name)
				{
					value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
					obj.name = value;
				}
				else if (key == SceneKey::Enabled)
					obj.enabled = std::stoi(value) != 0;
				else if (key == SceneKey::Parent)
					obj.parentIndex = std::stoi(value);
				else if (key == SceneKey::Position)
					ParseVec3(value, obj.position);
				else if (key == SceneKey::Scale)
					ParseVec3(value, obj.scale);
				else if (key == SceneKey::Rotation)
					ParseVec3(value, obj.rotation);
				else if (key == SceneKey::Children)
				{
					std::istringstream iss(value);
					int childIdx;
					while (iss >> childIdx)
						obj.childIndices.push_back(static_cast<ui32>(childIdx));
				}
			}
			else if (currentSection == SceneKey::Material)
			{
				obj.hasMaterial = true;
				if (key == SceneKey::Shader)
				{
					ParseQuotedPath(value, obj.materialShader);
				}
				else if (key == SceneKey::Color)
					ParseVec3(value, obj.materialColor);
				else if (key.starts_with(SceneKey::TexturePrefix))
				{
					ParseQuotedPath(value, obj.texture0);
				}
			}
			else if (currentSection == SceneKey::RigidBody)
			{
				obj.hasRigidBody = true;
				if (key == SceneKey::BodyType)
				{
					value.erase(std::remove(value.begin(), value.end(), '\"'), value.end());
					obj.bodyType = StringToBodyType(value);
				}
				else if (key == SceneKey::Mass)
					obj.mass = static_cast<f32>(std::stod(value));
				else if (key.starts_with(SceneKey::ColliderPrefix))
				{
					std::istringstream iss(value);
					std::string colliderType;
					iss >> colliderType;

					ColliderInfo info;
					if (colliderType == SceneKey::ColliderBox)
					{
						info.type = ColliderType::Box;
						iss >> info.halfExtents.x >> info.halfExtents.y >> info.halfExtents.z;
					}
					else if (colliderType == SceneKey::ColliderSphere)
					{
						info.type = ColliderType::Sphere;
						iss >> info.radius;
					}
					else if (colliderType == SceneKey::ColliderCapsule)
					{
						info.type = ColliderType::Capsule;
						iss >> info.radius >> info.height;
					}
					obj.colliders.push_back(info);
				}
			}
			else if (currentSection == SceneKey::Camera)
			{
				obj.hasCamera = true;
				if (key == SceneKey::NearPlane)
					obj.nearPlane = static_cast<f32>(std::stod(value));
				else if (key == SceneKey::FarPlane)
					obj.farPlane = static_cast<f32>(std::stod(value));
				else if (key == SceneKey::Fov)
					obj.fov = static_cast<f32>(std::stod(value));
				else if (key == SceneKey::Active)
					obj.activeCamera = std::stoi(value) != 0;
			}
			else if (currentSection == SceneKey::Controller)
			{
				obj.hasController = true;
				if (key == SceneKey::Sensitivity)
					obj.sensitivity = static_cast<f32>(std::stod(value));
				else if (key == SceneKey::Speed)
					obj.speedModifier = static_cast<f32>(std::stod(value));
			}
			else if (currentSection == SceneKey::MeshComponent)
			{
				obj.hasMeshComponent = true;
				if (key == SceneKey::Mesh)
				{
					ParseQuotedPath(value, obj.meshPath);
				}
				else if (key.starts_with(SceneKey::MaterialPrefix))
				{
					std::string shaderPath, texPath;
					Vec3 color{ 1.0f };

					size_t shaderStart = value.find('\"');
					if (shaderStart != std::string::npos)
					{
						size_t endQuote = value.find('\"', shaderStart + 1);
						if (endQuote != std::string::npos)
						{
							shaderPath = value.substr(shaderStart + 1, endQuote - shaderStart - 1);
							std::string rest = value.substr(endQuote + 1);

							std::istringstream restIss(rest);
							restIss >> color.x >> color.y >> color.z;

							std::wstring texturePath;
							if (ParseQuotedPath(rest, texturePath))
								texPath.assign(texturePath.begin(), texturePath.end());
						}
					}

					ParsedObject::MeshMatSlot slot;
					WidenPath(slot.shaderPath, shaderPath);
					slot.color = color;
					WidenPath(slot.texturePath, texPath);

					ui32 slotIdx = static_cast<ui32>(std::stoi(key.substr(std::string(SceneKey::MaterialPrefix).length())));
					if (slotIdx >= obj.meshMaterials.size())
						obj.meshMaterials.resize(slotIdx + 1);
					obj.meshMaterials[slotIdx] = slot;
				}
			}
		}

		file.close();

		std::vector<AGameObject*> createdObjects;
		createdObjects.resize(parsedObjects.size(), nullptr);

		for (size_t i = 0; i < parsedObjects.size(); ++i)
		{
			auto& obj = parsedObjects[i];

			AGameObject* created = nullptr;

			if (obj.typeId == Quad::getTypeId())
				created = world.CreateAGameObject<Quad>();
			else if (obj.typeId == Cube::getTypeId())
				created = world.CreateAGameObject<Cube>();
			else if (obj.typeId == Sphere::getTypeId())
				created = world.CreateAGameObject<Sphere>();
			else if (obj.typeId == Cylinder::getTypeId())
				created = world.CreateAGameObject<Cylinder>();
			else if (obj.typeId == Capsule::getTypeId())
				created = world.CreateAGameObject<Capsule>();
			else if (obj.typeId == MeshObject::getTypeId())
				created = world.CreateAGameObject<MeshObject>();
			else if (obj.typeId == CameraObject::getTypeId())
				created = world.CreateAGameObject<CameraObject>();
			else if (obj.typeId == Player::getTypeId())
				created = world.CreateAGameObject<Player>();
			else
				continue;

			createdObjects[i] = created;

			auto& transform = created->GetTransform();
			transform.SetPosition(obj.position);
			transform.SetScale(obj.scale);
			transform.SetRotation(obj.rotation);

			created->SetEnabled(obj.enabled);

			if (obj.hasMaterial)
			{
				auto& matComp = created->GetMaterialComponent();
				auto matRes = resourceManager.CreateResourceFromFile<MaterialResource>(obj.materialShader.c_str());
						if (matRes)
						{
							matRes->SetData(std::as_bytes(std::span{ &obj.materialColor, 1 }));
							if (!obj.texture0.empty())
							{
								auto tex = resourceManager.CreateResourceFromFile<TextureResource>(obj.texture0.c_str());
								if (tex)
									matRes->SetTexture(0, tex);
							}
							matComp.SetMaterial(matRes);
						}
			}

			if (obj.hasRigidBody)
		{
			auto* rb = created->CreateOrGetComponent<RigidBodyComponent>();
			rb->SetBodyType(obj.bodyType);
			for (auto& c : obj.colliders)
			{
				switch (c.type)
				{
				case ColliderType::Box:     rb->AddBoxCollider(c.halfExtents); break;
				case ColliderType::Sphere:  rb->AddSphereCollider(c.radius); break;
				case ColliderType::Capsule: rb->AddCapsuleCollider(c.radius, c.height); break;
				}
			}
			rb->SetMass(obj.mass);
			rb->SyncColliderScale();
		}

		if (obj.hasCamera)
		{
			auto* cam = created->CreateOrGetComponent<CameraComponent>();
			cam->SetNearPlane(obj.nearPlane);
			cam->SetFarPlane(obj.farPlane);
			cam->SetFieldOfView(obj.fov);
			cam->SetActive(obj.activeCamera);
		}

		if (obj.hasController)
		{
			auto* ctrl = created->CreateOrGetComponent<ControllerComponent>();
			ctrl->SetSensitivity(obj.sensitivity);
			ctrl->SetSpeedModifier(obj.speedModifier);
		}

			if (obj.hasMeshComponent)
			{
				auto* mc = created->CreateOrGetComponent<MeshComponent>();
						if (!obj.meshPath.empty())
				{
					auto mesh = resourceManager.CreateResourceFromFile<MeshResource>(obj.meshPath.c_str());
					if (mesh)
						mc->SetMesh(mesh);

							for (size_t m = 0; mesh && m < obj.meshMaterials.size(); ++m)
					{
						auto& slot = obj.meshMaterials[m];
						if (!slot.shaderPath.empty())
						{
							auto mat = resourceManager.CreateResourceFromFile<MaterialResource>(slot.shaderPath.c_str());
										if (mat)
							{
								mat->SetData(std::as_bytes(std::span{ &slot.color, 1 }));
								if (!slot.texturePath.empty())
								{
									auto tex = resourceManager.CreateResourceFromFile<TextureResource>(slot.texturePath.c_str());
									if (tex)
										mat->SetTexture(0, tex);
								}
								mc->SetMaterial(static_cast<ui32>(m), mat);
							}
						}
					}
				}
			}
		}

		for (size_t i = 0; i < parsedObjects.size(); ++i)
		{
			auto& obj = parsedObjects[i];
			if (obj.parentIndex >= 0 && obj.parentIndex < static_cast<int>(createdObjects.size()))
			{
				if (createdObjects[i] && createdObjects[obj.parentIndex])
					createdObjects[i]->SetParent(createdObjects[obj.parentIndex]);
			}
		}

		bool hasActiveCamera = false;
		for (auto* object : world.GetAllGameObjects())
		{
			if (object && object->GetTypeID() == CameraObject::getTypeId())
			{
				auto* camera = static_cast<CameraObject*>(object)->GetComponent<CameraComponent>();
				if (camera && camera->IsActive())
				{
					hasActiveCamera = true;
					break;
				}
			}
		}

		if (!hasActiveCamera)
		{
			for (auto* object : world.GetAllGameObjects())
			{
				if (object && object->GetTypeID() == CameraObject::getTypeId())
				{
					world.SetActiveCameraObject(static_cast<CameraObject*>(object));
					break;
				}
			}
		}

		return true;
	}

	std::unordered_map<size_t, std::unordered_map<SceneMode, TransformSnapshot>> SceneSerializer::ParseStates(const std::wstring& path)
	{
		std::unordered_map<size_t, std::unordered_map<SceneMode, TransformSnapshot>> result;
		return result;
	}
}
