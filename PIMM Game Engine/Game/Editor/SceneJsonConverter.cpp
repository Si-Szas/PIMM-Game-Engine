#include "SceneJsonConverter.h"
#include "SceneFormat.h"
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

#include <span>
#include <string>
#include <unordered_map>
#include <vector>

namespace pimm
{
	namespace
	{
		std::string Narrow(const std::wstring& w)
		{
			return std::string(w.begin(), w.end());
		}

		std::wstring Widen(const std::string& s)
		{
			return std::wstring(s.begin(), s.end());
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

		std::string GetTypeName(size_t typeId)
		{
			static const std::unordered_map<size_t, std::string> map = {
				{ Quad::getTypeId(),       "Quad" },
				{ Cube::getTypeId(),       "Cube" },
				{ Sphere::getTypeId(),     "Sphere" },
				{ Cylinder::getTypeId(),   "Cylinder" },
				{ Capsule::getTypeId(),    "Capsule" },
				{ MeshObject::getTypeId(), "MeshObject" },
				{ CameraObject::getTypeId(), "CameraObject" },
				{ Player::getTypeId(),     "Player" },
			};

			auto it = map.find(typeId);
			return (it != map.end()) ? it->second : "Unknown";
		}

		size_t GetTypeIdFromName(const std::string& name)
		{
			static const std::unordered_map<std::string, size_t> map = {
				{ "Quad",         Quad::getTypeId()       },
				{ "Cube",         Cube::getTypeId()       },
				{ "Sphere",       Sphere::getTypeId()     },
				{ "Cylinder",     Cylinder::getTypeId()   },
				{ "Capsule",      Capsule::getTypeId()    },
				{ "MeshObject",   MeshObject::getTypeId() },
				{ "CameraObject", CameraObject::getTypeId() },
				{ "Player",       Player::getTypeId()     },
			};

			auto it = map.find(name);
			return (it != map.end()) ? it->second : 0;
		}

		nlohmann::json Vec3ToJson(const Vec3& v)
		{
			nlohmann::json j = nlohmann::json::array();
			j.push_back(v.x);
			j.push_back(v.y);
			j.push_back(v.z);
			return j;
		}

		Vec3 Vec3FromJson(const nlohmann::json& j, const Vec3& fallback)
		{
			if (!j.is_array() || j.size() < 3)
				return fallback;

			return Vec3{ j[0].get<f32>(), j[1].get<f32>(), j[2].get<f32>() };
		}
	}

	SceneData WorldToSceneData(const World& world)
	{
		auto objects = world.GetAllGameObjects();

		std::vector<int> remap(objects.size(), -1);

		SceneData data;
		data.objects.reserve(objects.size());

		for (ui32 i = 0; i < objects.size(); ++i)
		{
			auto* obj = objects[i];
			if (!obj)
				continue;
			if (obj->GetTypeID() == 0)
				continue;
			if (obj->GetTypeID() == Player::getTypeId())
				continue;

			remap[i] = static_cast<int>(data.objects.size());

			ObjectData entry;
			entry.typeId = obj->GetTypeID();
			entry.name = obj->GetObjectName();
			entry.enabled = obj->IsEnabled();

			const auto& transform = obj->GetTransform();
			entry.position = transform.GetPosition();
			entry.scale = transform.GetScale();
			entry.rotation = transform.GetRotation();

			const auto& components = obj->GetAllComponents();
			for (const auto& [compId, comp] : components)
			{
				if (!comp)
					continue;

				if (compId == MaterialComponent::getTypeId())
				{
					MaterialData material;
					auto& mc = *static_cast<MaterialComponent*>(comp.get());
					auto* mat = mc.GetMaterial();
					if (!mat)
						continue;

					material.shader = mat->GetPath();

					auto matData = mat->GetData();
					if (matData.size() >= sizeof(Vec3))
						material.color = *reinterpret_cast<const Vec3*>(matData.data());

					auto* tex = mat->GetTexture(0);
					if (tex)
						material.texture0 = tex->GetPath();

					entry.material = std::move(material);
				}
				else if (compId == RigidBodyComponent::getTypeId())
				{
					RigidBodyData rigidBody;
					auto& rb = *static_cast<RigidBodyComponent*>(comp.get());
					rigidBody.bodyType = rb.GetBodyType();
					rigidBody.mass = rb.GetMass();
					for (ui32 c = 0; c < rb.GetColliderCount(); ++c)
						rigidBody.colliders.push_back(rb.GetCollider(c));
					entry.rigidBody = std::move(rigidBody);
				}
				else if (compId == CameraComponent::getTypeId())
				{
					CameraData camera;
					auto& cam = *static_cast<CameraComponent*>(comp.get());
					camera.nearPlane = cam.GetNearPlane();
					camera.farPlane = cam.GetFarPlane();
					camera.fov = cam.GetFieldOfView();
					camera.active = cam.IsActive();
					entry.camera = std::move(camera);
				}
				else if (compId == ControllerComponent::getTypeId())
				{
					ControllerData controller;
					auto& ctrl = *static_cast<ControllerComponent*>(comp.get());
					controller.sensitivity = ctrl.GetSensitivity();
					controller.speed = ctrl.GetSpeedModifier();
					entry.controller = std::move(controller);
				}
				else if (compId == MeshComponent::getTypeId())
				{
					MeshComponentData meshComponent;
					auto& mc = *static_cast<MeshComponent*>(comp.get());

					auto* mesh = mc.GetMesh();
					if (mesh)
						meshComponent.meshPath = mesh->GetPath();

					ui32 materialCount = mc.GetMaterialCount();
					for (ui32 m = 0; m < materialCount; ++m)
					{
						auto* mat = mc.GetMaterial(m);
						if (!mat)
							continue;

						MeshMatSlot slot;
						slot.shaderPath = mat->GetPath();

						auto matData = mat->GetData();
						if (matData.size() >= sizeof(Vec3))
							slot.color = *reinterpret_cast<const Vec3*>(matData.data());

						auto* tex = mat->GetTexture(0);
						if (tex)
							slot.texturePath = tex->GetPath();

						meshComponent.materials.push_back(std::move(slot));
					}

					entry.meshComponent = std::move(meshComponent);
				}
			}

			data.objects.push_back(std::move(entry));
		}

		for (ui32 i = 0; i < objects.size(); ++i)
		{
			auto* obj = objects[i];
			if (!obj || remap[i] < 0)
				continue;

			auto& entry = data.objects[static_cast<size_t>(remap[i])];

			auto* parent = obj->GetParent();
			if (parent)
			{
				for (ui32 p = 0; p < objects.size(); ++p)
				{
					if (objects[p] == parent)
					{
						if (remap[p] >= 0)
							entry.parentIndex = remap[p];
						break;
					}
				}
			}

			const auto& children = obj->GetChildren();
			for (auto* child : children)
			{
				for (ui32 c = 0; c < objects.size(); ++c)
				{
					if (objects[c] == child)
					{
						if (remap[c] >= 0)
							entry.children.push_back(static_cast<ui32>(remap[c]));
						break;
					}
				}
			}
		}

		return data;
	}

	void SceneDataToWorld(World& world, const SceneData& data, ResourceManager& resourceManager)
	{
		world.DeleteAllAGameObjects();

		std::vector<AGameObject*> createdObjects;
		createdObjects.resize(data.objects.size(), nullptr);

		for (size_t i = 0; i < data.objects.size(); ++i)
		{
			const auto& obj = data.objects[i];

			if (obj.typeId == Player::getTypeId())
				continue;

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

			created->SetObjectName(obj.name);
			created->SetEnabled(obj.enabled);

			if (obj.material)
			{
				auto& matComp = created->GetMaterialComponent();
				auto matRes = resourceManager.CreateResourceFromFile<MaterialResource>(obj.material->shader.c_str());
				if (matRes)
				{
					matRes->SetData(std::as_bytes(std::span{ &obj.material->color, 1 }));
					if (!obj.material->texture0.empty())
					{
						auto tex = resourceManager.CreateResourceFromFile<TextureResource>(obj.material->texture0.c_str());
						if (tex)
							matRes->SetTexture(0, tex);
					}
					matComp.SetMaterial(matRes);
				}
			}

			if (obj.rigidBody)
			{
				auto* rb = created->CreateOrGetComponent<RigidBodyComponent>();
				rb->SetBodyType(obj.rigidBody->bodyType);
				for (auto& c : obj.rigidBody->colliders)
				{
					switch (c.type)
					{
					case ColliderType::Box:     rb->AddBoxCollider(c.halfExtents, c.position, c.rotation); break;
					case ColliderType::Sphere:  rb->AddSphereCollider(c.radius, c.position, c.rotation); break;
					case ColliderType::Capsule: rb->AddCapsuleCollider(c.radius, c.height, c.position, c.rotation); break;
					}
				}
				rb->SetMass(obj.rigidBody->mass);
				rb->SyncColliderScale();
			}

			if (obj.camera)
			{
				auto* cam = created->CreateOrGetComponent<CameraComponent>();
				cam->SetNearPlane(obj.camera->nearPlane);
				cam->SetFarPlane(obj.camera->farPlane);
				cam->SetFieldOfView(obj.camera->fov);
				cam->SetActive(obj.camera->active);
			}

			if (obj.controller)
			{
				auto* ctrl = created->CreateOrGetComponent<ControllerComponent>();
				ctrl->SetSensitivity(obj.controller->sensitivity);
				ctrl->SetSpeedModifier(obj.controller->speed);
			}

			if (obj.meshComponent)
			{
				auto* mc = created->CreateOrGetComponent<MeshComponent>();
				if (!obj.meshComponent->meshPath.empty())
				{
					auto mesh = resourceManager.CreateResourceFromFile<MeshResource>(obj.meshComponent->meshPath.c_str());
					if (mesh)
					{
						mc->SetMesh(mesh);
						for (size_t m = 0; m < obj.meshComponent->materials.size(); ++m)
						{
							const auto& slot = obj.meshComponent->materials[m];
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
		}

		for (size_t i = 0; i < data.objects.size(); ++i)
		{
			const auto& obj = data.objects[i];
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
	}

	nlohmann::json ToJson(const SceneData& data)
	{
		nlohmann::json root = nlohmann::json::object();
		root[SceneKey::Version] = data.version;

		nlohmann::json objects = nlohmann::json::array();

		for (const auto& obj : data.objects)
		{
			nlohmann::json entry = nlohmann::json::object();
			entry[SceneKey::Type] = GetTypeName(obj.typeId);
			entry[SceneKey::Name] = obj.name;
			entry[SceneKey::Enabled] = obj.enabled;
			entry[SceneKey::Parent] = obj.parentIndex;
			entry[SceneKey::Position] = Vec3ToJson(obj.position);
			entry[SceneKey::Scale] = Vec3ToJson(obj.scale);
			entry[SceneKey::Rotation] = Vec3ToJson(obj.rotation);

			if (!obj.children.empty())
				entry[SceneKey::Children] = obj.children;

			if (obj.material)
			{
				nlohmann::json material = nlohmann::json::object();
				material[SceneKey::Shader] = Narrow(obj.material->shader);
				material[SceneKey::Color] = Vec3ToJson(obj.material->color);
				if (!obj.material->texture0.empty())
					material[std::string(SceneKey::TexturePrefix) + "0"] = Narrow(obj.material->texture0);
				entry[SceneKey::Material] = std::move(material);
			}

			if (obj.rigidBody)
			{
				nlohmann::json rigidBody = nlohmann::json::object();
				rigidBody[SceneKey::BodyType] = BodyTypeToString(obj.rigidBody->bodyType);
				rigidBody[SceneKey::Mass] = obj.rigidBody->mass;

				nlohmann::json colliders = nlohmann::json::array();
				for (const auto& collider : obj.rigidBody->colliders)
				{
					nlohmann::json colliderJson = nlohmann::json::object();
					colliderJson[SceneKey::Type] = ColliderTypeToString(collider.type);
					switch (collider.type)
					{
					case ColliderType::Box:
						colliderJson[SceneKey::HalfExtents] = Vec3ToJson(collider.halfExtents);
						break;
					case ColliderType::Sphere:
						colliderJson[SceneKey::Radius] = collider.radius;
						break;
					case ColliderType::Capsule:
						colliderJson[SceneKey::Radius] = collider.radius;
						colliderJson[SceneKey::Height] = collider.height;
						break;
					}
					colliderJson[SceneKey::Position] = Vec3ToJson(collider.position);
					colliderJson[SceneKey::Rotation] = Vec3ToJson(collider.rotation);
					colliders.push_back(std::move(colliderJson));
				}
				rigidBody[SceneKey::ColliderPrefix] = std::move(colliders);
				entry[SceneKey::RigidBody] = std::move(rigidBody);
			}

			if (obj.camera)
			{
				nlohmann::json camera = nlohmann::json::object();
				camera[SceneKey::NearPlane] = obj.camera->nearPlane;
				camera[SceneKey::FarPlane] = obj.camera->farPlane;
				camera[SceneKey::Fov] = obj.camera->fov;
				camera[SceneKey::Active] = obj.camera->active;
				entry[SceneKey::Camera] = std::move(camera);
			}

			if (obj.controller)
			{
				nlohmann::json controller = nlohmann::json::object();
				controller[SceneKey::Sensitivity] = obj.controller->sensitivity;
				controller[SceneKey::Speed] = obj.controller->speed;
				entry[SceneKey::Controller] = std::move(controller);
			}

			if (obj.meshComponent)
			{
				nlohmann::json meshComponent = nlohmann::json::object();
				if (!obj.meshComponent->meshPath.empty())
					meshComponent[SceneKey::Mesh] = Narrow(obj.meshComponent->meshPath);

				nlohmann::json materials = nlohmann::json::array();
				for (const auto& slot : obj.meshComponent->materials)
				{
					nlohmann::json slotJson = nlohmann::json::object();
					slotJson[SceneKey::Shader] = Narrow(slot.shaderPath);
					slotJson[SceneKey::Color] = Vec3ToJson(slot.color);
					if (!slot.texturePath.empty())
						slotJson[SceneKey::TexturePrefix] = Narrow(slot.texturePath);
					materials.push_back(std::move(slotJson));
				}
				meshComponent[SceneKey::MaterialPrefix] = std::move(materials);
				entry[SceneKey::MeshComponent] = std::move(meshComponent);
			}

			objects.push_back(std::move(entry));
		}

		root[SceneKey::Objects] = std::move(objects);
		return root;
	}

	SceneData FromJson(const nlohmann::json& document)
	{
		SceneData data;
		data.version = document.value(SceneKey::Version, 1u);

		const auto& objectsJson = document.value(SceneKey::Objects, nlohmann::json::array());
		for (const auto& itemJson : objectsJson)
		{
			if (!itemJson.is_object())
				continue;

			ObjectData obj;
			obj.typeId = GetTypeIdFromName(itemJson.value(SceneKey::Type, std::string{}));
			obj.name = itemJson.value(SceneKey::Name, std::string{});
			obj.enabled = itemJson.value(SceneKey::Enabled, true);
			obj.parentIndex = itemJson.value(SceneKey::Parent, -1);
			obj.position = Vec3FromJson(itemJson.value(SceneKey::Position, nlohmann::json::array()), Vec3{ 0.0f });
			obj.scale = Vec3FromJson(itemJson.value(SceneKey::Scale, nlohmann::json::array()), Vec3{ 1.0f });
			obj.rotation = Vec3FromJson(itemJson.value(SceneKey::Rotation, nlohmann::json::array()), Vec3{ 0.0f });

			if (itemJson.contains(SceneKey::Children) && itemJson[SceneKey::Children].is_array())
			{
				for (const auto& child : itemJson[SceneKey::Children])
				{
					if (child.is_number())
						obj.children.push_back(child.get<ui32>());
				}
			}

			if (itemJson.contains(SceneKey::Material) && itemJson[SceneKey::Material].is_object())
			{
				const auto& materialJson = itemJson[SceneKey::Material];

				MaterialData material;
				material.shader = Widen(materialJson.value(SceneKey::Shader, std::string{}));
				material.color = Vec3FromJson(materialJson.value(SceneKey::Color, nlohmann::json::array()), Vec3{ 1.0f });

				for (auto it = materialJson.begin(); it != materialJson.end(); ++it)
				{
					if (it.key().starts_with(SceneKey::TexturePrefix) && it.value().is_string())
						material.texture0 = Widen(it.value().get<std::string>());
				}

				obj.material = std::move(material);
			}

			if (itemJson.contains(SceneKey::RigidBody) && itemJson[SceneKey::RigidBody].is_object())
			{
				const auto& rigidBodyJson = itemJson[SceneKey::RigidBody];

				RigidBodyData rigidBody;
				rigidBody.bodyType = StringToBodyType(rigidBodyJson.value(SceneKey::BodyType, std::string{}));
				rigidBody.mass = rigidBodyJson.value(SceneKey::Mass, 1.0f);

				if (rigidBodyJson.contains(SceneKey::ColliderPrefix) && rigidBodyJson[SceneKey::ColliderPrefix].is_array())
				{
					for (const auto& colliderJson : rigidBodyJson[SceneKey::ColliderPrefix])
					{
						if (!colliderJson.is_object())
							continue;

						const std::string colliderType = colliderJson.value(SceneKey::Type, std::string{});

						ColliderInfo info;
						if (colliderType == SceneKey::ColliderBox)
						{
							info.type = ColliderType::Box;
							info.halfExtents = Vec3FromJson(colliderJson.value(SceneKey::HalfExtents, nlohmann::json::array()), info.halfExtents);
						}
						else if (colliderType == SceneKey::ColliderSphere)
						{
							info.type = ColliderType::Sphere;
							info.radius = colliderJson.value(SceneKey::Radius, info.radius);
						}
						else if (colliderType == SceneKey::ColliderCapsule)
						{
							info.type = ColliderType::Capsule;
							info.radius = colliderJson.value(SceneKey::Radius, info.radius);
							info.height = colliderJson.value(SceneKey::Height, info.height);
						}
						else
						{
							continue;
						}

						info.position = Vec3FromJson(colliderJson.value(SceneKey::Position, nlohmann::json::array()), Vec3{ 0.0f });
						info.rotation = Vec3FromJson(colliderJson.value(SceneKey::Rotation, nlohmann::json::array()), Vec3{ 0.0f });

						rigidBody.colliders.push_back(info);
					}
				}

				obj.rigidBody = std::move(rigidBody);
			}

			if (itemJson.contains(SceneKey::Camera) && itemJson[SceneKey::Camera].is_object())
			{
				const auto& cameraJson = itemJson[SceneKey::Camera];

				CameraData camera;
				camera.nearPlane = cameraJson.value(SceneKey::NearPlane, 0.01f);
				camera.farPlane = cameraJson.value(SceneKey::FarPlane, 100.0f);
				camera.fov = cameraJson.value(SceneKey::Fov, 1.5f);
				camera.active = cameraJson.value(SceneKey::Active, false);

				obj.camera = std::move(camera);
			}

			if (itemJson.contains(SceneKey::Controller) && itemJson[SceneKey::Controller].is_object())
			{
				const auto& controllerJson = itemJson[SceneKey::Controller];

				ControllerData controller;
				controller.sensitivity = controllerJson.value(SceneKey::Sensitivity, 0.1f);
				controller.speed = controllerJson.value(SceneKey::Speed, 3.0f);

				obj.controller = std::move(controller);
			}

			if (itemJson.contains(SceneKey::MeshComponent) && itemJson[SceneKey::MeshComponent].is_object())
			{
				const auto& meshJson = itemJson[SceneKey::MeshComponent];

				MeshComponentData meshComponent;
				meshComponent.meshPath = Widen(meshJson.value(SceneKey::Mesh, std::string{}));

				if (meshJson.contains(SceneKey::MaterialPrefix) && meshJson[SceneKey::MaterialPrefix].is_array())
				{
					for (const auto& slotJson : meshJson[SceneKey::MaterialPrefix])
					{
						if (!slotJson.is_object())
							continue;

						MeshMatSlot slot;
						slot.shaderPath = Widen(slotJson.value(SceneKey::Shader, std::string{}));
						slot.color = Vec3FromJson(slotJson.value(SceneKey::Color, nlohmann::json::array()), Vec3{ 1.0f });
						slot.texturePath = Widen(slotJson.value(SceneKey::TexturePrefix, std::string{}));

						meshComponent.materials.push_back(std::move(slot));
					}
				}

				obj.meshComponent = std::move(meshComponent);
			}

			data.objects.push_back(std::move(obj));
		}

		return data;
	}
}
