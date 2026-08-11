#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Game/World.h>
#include <PIMM/ImGui/imgui.h>
#include "../Player/Player.h"
#include "../Editor/SceneModeManager.h"

#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/RigidBodyComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AComponent/ControllerComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>
#include <PIMM/AComponent/MeshComponent.h>
#include <PIMM/Resource/MeshResource.h>
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Resource/TextureResource.h>
#include <PIMM/Resource/ResourceManager.h>
#include <PIMM/AGameObject/MeshObject.h>
#include <PIMM/AGameObject/CameraObject.h>

#include <filesystem>
#include <vector>
#include <string>
#include <functional>

namespace pimm
{
	class InspectorPanel final : public APanel
	{
	public:
		explicit InspectorPanel(World& world, const SceneModeManager& modeManager) : APanel("Inspector"), m_world(world), m_modeManager(modeManager) {}

		void Render() override
		{
			ImGui::Begin("Inspector");
			AGameObject* gameObject = m_world.GetSelectedGameObject();
			bool isEditMode = m_modeManager.IsEditMode();
			bool removeRigidBodyRequested = false;
			bool removeControllerRequested = false;

			if(gameObject){
				size_t objectType = gameObject->GetTypeID();

				ImGui::Text(gameObject->GetObjectLabel(gameObject));
				ImGui::NewLine();

				///////////COMPONENTS///////////
				// TRANSFORM

				const auto& components = gameObject->GetAllComponents();

				for (const auto& [componentId, componentPtr] : components)
				{
					// Make sure the component exists first
					if (componentPtr)
					{
						//Transform Component
						if (componentId == pimm::TransformComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("Transform");
							ImGui::NewLine();

							auto& transform = gameObject->GetTransform();

							Vec3 position = transform.GetPosition();
							Vec3 rotation = transform.GetRotation();
							Vec3 scale = transform.GetScale();

							float positionArr[3] = { position.x, position.y, position.z };
							float rotationArr[3] = { rotation.x, rotation.y, rotation.z };
							float scaleArr[3] = { scale.x, scale.y, scale.z };

							//If the scene is in edit mode, allow the player to manipulate its transform component
							if (isEditMode)
							{
								if (ImGui::DragFloat3("Position", positionArr, 0.1f))
									transform.SetPosition({ positionArr[0], positionArr[1], positionArr[2] });

								if (ImGui::DragFloat3("Rotation", rotationArr, 0.1f))
									transform.SetRotation({ rotationArr[0], rotationArr[1], rotationArr[2] });

								if (ImGui::DragFloat3("Scale", scaleArr, 0.1f))
									transform.SetScale({ scaleArr[0], scaleArr[1], scaleArr[2] });
							}
							// If scene is running, do not allow manipulation
							else
							{
								ImGui::InputFloat3("Position", positionArr, "%.2f", ImGuiInputTextFlags_ReadOnly);
								ImGui::InputFloat3("Rotation", rotationArr, "%.2f", ImGuiInputTextFlags_ReadOnly);
								ImGui::InputFloat3("Scale", scaleArr, "%.2f", ImGuiInputTextFlags_ReadOnly);
							}
						}


						if (componentId == pimm::CameraComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("Camera");
							ImGui::NewLine();

							auto* camera = static_cast<pimm::CameraComponent*>(componentPtr.get());
							bool active = camera->IsActive();
							if (isEditMode && ImGui::Checkbox("Active Scene Camera", &active) && active)
								m_world.SetActiveCameraObject(static_cast<pimm::CameraObject*>(gameObject));

							float nearPlane = camera->GetNearPlane();
							float farPlane = camera->GetFarPlane();
							float fieldOfView = camera->GetFieldOfView();
							if (isEditMode)
							{
								if (ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.001f, farPlane))
									camera->SetNearPlane(nearPlane);
								if (ImGui::DragFloat("Far Plane", &farPlane, 1.0f, nearPlane, 100000.0f))
									camera->SetFarPlane(farPlane);
								if (ImGui::DragFloat("Field Of View", &fieldOfView, 0.01f, 0.001f, 3.13f))
									camera->SetFieldOfView(fieldOfView);
							}
							else
							{
								ImGui::Text("Near Plane: %.2f", nearPlane);
								ImGui::Text("Far Plane: %.2f", farPlane);
								ImGui::Text("Field Of View: %.2f", fieldOfView);
							}
						}

						// Checking if it is a mesh object or not since materials are handled slightly differently
						if (objectType != pimm::MeshObject::getTypeId())
						{
							if (componentId == pimm::MaterialComponent::getTypeId())
							{
								ImGui::Separator();
								ImGui::Text("Material");
								ImGui::NewLine();

								//Get the material component and resource
								auto& material = gameObject->GetMaterialComponent();
								auto* materialResource = material.GetMaterial();
								//Ensure material data exists
								if (materialResource)
								{
									auto materialData = materialResource->GetData();

									if (materialData.size() >= sizeof(pimm::Vec3))
									{
										const pimm::Vec3* colorVector = reinterpret_cast<const pimm::Vec3*>(materialData.data());

										float color[3] = { colorVector->x, colorVector->y, colorVector->z };

										if (isEditMode)
										{
											if (ImGui::ColorEdit3("Material Color", color))
											{
												pimm::Vec3 updatedColor(color[0], color[1], color[2]);
												auto clonedMaterial = CloneMaterialForObject(gameObject, materialResource);
												clonedMaterial->SetData(std::as_bytes(std::span{ &updatedColor, 1 }));
												material.SetMaterial(clonedMaterial);
												materialResource = clonedMaterial.get();
											}
										}
										else
										{
											ImGui::ColorButton("Material Color", ImVec4(color[0], color[1], color[2], 1.0f), ImGuiColorEditFlags_NoTooltip);
											ImGui::SameLine();
											ImGui::Text("R:%.2f G:%.2f B:%.2f", color[0], color[1], color[2]);
										}
									}


									DrawTextureSlots(gameObject, materialResource,
										[&material](const RefPtr<MaterialResource>& cloned)
										{
											material.SetMaterial(cloned);
										});
								}
							}
						} //Means the game object is a mesh object
						else {
							if (componentId == pimm::MeshComponent::getTypeId())
							{
								ImGui::Separator();
								ImGui::Text("Material");
								ImGui::NewLine();

								auto meshComponent = gameObject->CreateOrGetComponent<MeshComponent>();
								auto* meshResource = meshComponent->GetMesh();
								//Make sure mesh resource exists
								if (meshResource)
								{
									size_t materialSlots = meshResource->GetNumberOfMaterialSlots();

									for (size_t i = 0; i < materialSlots; ++i)
									{
										pimm::MaterialResource* materialResource = meshComponent->GetMaterial(static_cast<ui32>(i));

										if (materialResource)
										{
											auto materialData = materialResource->GetData();

											if (materialData.size() >= sizeof(pimm::Vec3))
											{
												const pimm::Vec3* colorVector = reinterpret_cast<const pimm::Vec3*>(materialData.data());
												float color[3] = { colorVector->x, colorVector->y, colorVector->z };

												if (isEditMode)
												{
													if (ImGui::ColorEdit3("Material Color", color))
													{
														pimm::Vec3 updatedColor(color[0], color[1], color[2]);
														materialResource->SetData(std::as_bytes(std::span{ &updatedColor, 1 }));
													}
												}
												else
												{
													ImGui::ColorButton("Material Color", ImVec4(color[0], color[1], color[2], 1.0f), ImGuiColorEditFlags_NoTooltip);
													ImGui::SameLine();
													ImGui::Text("R:%.2f G:%.2f B:%.2f", color[0], color[1], color[2]);
												}
											}

											ImGui::PushID(static_cast<int>(i));

											ui32 slotIndex = static_cast<ui32>(i);
											DrawTextureSlots(gameObject, materialResource,
												[meshComponent, slotIndex](const RefPtr<MaterialResource>& cloned)
												{
													meshComponent->SetMaterial(slotIndex, cloned);
												});
											ImGui::PopID();
										}
									}
								}
							}
						}

						//Rigid Body Component
						if (componentId == pimm::RigidBodyComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("RigidBody");
							ImGui::NewLine();

							auto* rigidBody = static_cast<pimm::RigidBodyComponent*>(componentPtr.get());

							if (isEditMode)
							{
								static const char* bodyTypeNames[] = { "Static", "Kinematic", "Dynamic" };
								int currentBodyType = static_cast<int>(rigidBody->GetBodyType());

								if (ImGui::Combo("Body Type", &currentBodyType, bodyTypeNames, IM_ARRAYSIZE(bodyTypeNames)))
								{
									rigidBody->SetBodyType(static_cast<pimm::BodyType>(currentBodyType));
								}
							}
							else
							{
								static const char* bodyTypeNames[] = { "Static", "Kinematic", "Dynamic" };
								ImGui::Text("Body Type: %s", bodyTypeNames[static_cast<int>(rigidBody->GetBodyType())]);
							}

							// Gravity toggle 
							if (rigidBody->GetBodyType() == pimm::BodyType::Dynamic)
							{
								bool gravityEnabled = rigidBody->IsGravityEnabled();

								if (isEditMode)
								{
									if (ImGui::Checkbox("Use Gravity", &gravityEnabled))
									{
										rigidBody->EnableGravity(gravityEnabled);
									}
								}
								else
								{
									ImGui::Text(gravityEnabled ? "Gravity: Enabled" : "Gravity: Disabled");
								}
							}

							if (isEditMode)
							{
								ImGui::NewLine();
								if (ImGui::Button("Remove RigidBody Component"))
									removeRigidBodyRequested = true;
							}
						}

						//Collider
						if (componentId == pimm::RigidBodyComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("Collider");
							ImGui::NewLine();

							auto* rigidBody = static_cast<pimm::RigidBodyComponent*>(componentPtr.get());
							bool hasCollider = rigidBody->GetColliderCount() > 0;

							if (isEditMode)
							{
								if (ImGui::Checkbox("Enable Collider", &hasCollider))
								{
									if (hasCollider)
										rigidBody->RestoreLastCollider();
									else
										rigidBody->RemoveAllColliders();
								}
							}
							else
							{
								ImGui::Text(hasCollider ? "Collider: Enabled" : "Collider: Disabled");
							}

							if (hasCollider && rigidBody->GetColliderCount() > 0)
							{
								const ColliderInfo& info = rigidBody->GetCurrentColliderInfo(0);
								static const char* colliderTypeNames[] = { "Box", "Sphere", "Capsule" };
								int currentType = static_cast<int>(info.type);

								if (isEditMode)
								{
									if (ImGui::Combo("Collider Type", &currentType, colliderTypeNames, IM_ARRAYSIZE(colliderTypeNames)))
									{
										rigidBody->RemoveAllColliders();
										switch (static_cast<ColliderType>(currentType))
										{
										case ColliderType::Box:     rigidBody->AddBoxCollider(Vec3{ 0.5f });       break;
										case ColliderType::Sphere:  rigidBody->AddSphereCollider(0.5f);            break;
										case ColliderType::Capsule: rigidBody->AddCapsuleCollider(0.5f, 1.0f);     break;
										}
									}

									const ColliderInfo& current = rigidBody->GetCurrentColliderInfo(0);

									if (current.type == ColliderType::Box)
									{
										float halfExtents[3] = {
											current.halfExtents.x,
											current.halfExtents.y,
											current.halfExtents.z
										};

										if (ImGui::DragFloat3("Half Extents", halfExtents, 0.05f, 0.01f, 1000.0f))
										{
											halfExtents[0] = std::max(halfExtents[0], 0.01f);
											halfExtents[1] = std::max(halfExtents[1], 0.01f);
											halfExtents[2] = std::max(halfExtents[2], 0.01f);

											rigidBody->RemoveAllColliders();
											rigidBody->AddBoxCollider({
												halfExtents[0],
												halfExtents[1],
												halfExtents[2]
												});
										}
									}
									else if (current.type == ColliderType::Sphere)
									{
										float radius = current.radius;

										if (ImGui::DragFloat("Radius", &radius, 0.05f, 0.01f, 1000.0f))
										{
											radius = std::max(radius, 0.01f);

											rigidBody->RemoveAllColliders();
											rigidBody->AddSphereCollider(radius);
										}
									}
									else if (current.type == ColliderType::Capsule)
									{
										float radius = current.radius;
										float height = current.height;

										bool changed = false;

										changed |= ImGui::DragFloat("Radius", &radius, 0.05f, 0.01f, 1000.0f);
										changed |= ImGui::DragFloat("Height", &height, 0.05f, 0.01f, 1000.0f);

										if (changed)
										{
											radius = std::max(radius, 0.01f);
											height = std::max(height, 0.01f);

											rigidBody->RemoveAllColliders();
											rigidBody->AddCapsuleCollider(radius, height);
										}
									}
								}
								else
								{
									ImGui::Text("Type: %s", colliderTypeNames[currentType]);
									if (info.type == ColliderType::Box)
										ImGui::Text("Half Extents: %.2f, %.2f, %.2f", info.halfExtents.x, info.halfExtents.y, info.halfExtents.z);
									else if (info.type == ColliderType::Sphere)
										ImGui::Text("Radius: %.2f", info.radius);
									else if (info.type == ColliderType::Capsule)
										ImGui::Text("Radius: %.2f  Height: %.2f", info.radius, info.height);
								}
							}
						}
						if (componentId == pimm::ControllerComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("Controller");
							ImGui::NewLine();
							if (isEditMode)
							{
								if (ImGui::Button("Remove Controller Component"))
									removeControllerRequested = true;
							}
						}
					}
				}
				if (removeRigidBodyRequested)
				{
					gameObject->RemoveComponent<RigidBodyComponent>();
				}
				else if (isEditMode && !gameObject->GetComponent<RigidBodyComponent>())
				{
					ImGui::Separator();
					if (ImGui::Button("Add RigidBody Component"))
						gameObject->CreateOrGetComponent<RigidBodyComponent>();
				}

				if (removeControllerRequested)
				{
					gameObject->RemoveComponent<ControllerComponent>();
				}
				else if (isEditMode && objectType == CameraObject::getTypeId() && !gameObject->GetComponent<ControllerComponent>())
				{
					ImGui::Separator();
					if (ImGui::Button("Add Controller Component"))
						gameObject->CreateOrGetComponent<ControllerComponent>();
				}

			}

			ImGui::End();
		}

	private:

		struct TextureEntry
		{
			std::string displayName;
			std::wstring fullPath;
		};

		const std::vector<TextureEntry>& GetAvailableTextures()
		{
			static std::vector<TextureEntry> cachedTextures = []
				{
					std::vector<TextureEntry> result;
					std::filesystem::path texturesDir = L"Game/Assets/Textures";

					if (std::filesystem::exists(texturesDir) && std::filesystem::is_directory(texturesDir))
					{
						for (const auto& entry : std::filesystem::directory_iterator(texturesDir))
						{
							if (!entry.is_regular_file()) continue;
							result.push_back(TextureEntry{
								entry.path().filename().string(),
								entry.path().wstring()
								});
						}
					}

					return result;
				}();

			return cachedTextures;
		}

		void DrawTextureSlots(pimm::AGameObject* gameObject, pimm::MaterialResource*& materialResource,
			const std::function<void(const RefPtr<MaterialResource>&)>& reassign)
		{
			const auto& availableTextures = GetAvailableTextures();
			size_t textureSlots = materialResource->GetNumberOfTextures();

			for (size_t slot = 0; slot < textureSlots; ++slot)
			{
				ImGui::PushID(static_cast<int>(slot));

				pimm::TextureResource* currentTexture = materialResource->GetTexture(slot);
				std::string currentLabel = "None";
				if (currentTexture)
				{
					currentLabel = std::filesystem::path(currentTexture->GetPath()).filename().string();
				}

				std::string comboLabel = "Texture Slot " + std::to_string(slot);

				if (ImGui::BeginCombo(comboLabel.c_str(), currentLabel.c_str()))
				{
					bool noneSelected = (currentTexture == nullptr);
					if (ImGui::Selectable("None", noneSelected))
					{
						auto clonedMaterial = CloneMaterialForObject(gameObject, materialResource);
						clonedMaterial->SetTexture(slot, nullptr);
						reassign(clonedMaterial);
						materialResource = clonedMaterial.get();
					}

					for (const auto& textureEntry : availableTextures)
					{
						bool isSelected = (currentTexture != nullptr && currentLabel == textureEntry.displayName);
						if (ImGui::Selectable(textureEntry.displayName.c_str(), isSelected))
						{
							auto& resourceManager = gameObject->GetResourceManager();
							auto newTexture = resourceManager.CreateResourceFromFile<pimm::TextureResource>(textureEntry.fullPath.c_str());

							auto clonedMaterial = CloneMaterialForObject(gameObject, materialResource);
							clonedMaterial->SetTexture(slot, newTexture);
							reassign(clonedMaterial);
							materialResource = clonedMaterial.get();
						}
						if (isSelected)
						{
							ImGui::SetItemDefaultFocus();
						}
					}

					ImGui::EndCombo();
				}

				ImGui::PopID();
			}
		}

		RefPtr<MaterialResource> CloneMaterialForObject(pimm::AGameObject* gameObject, pimm::MaterialResource* source)
		{
			MaterialResourceDescriptor descriptor{
				{ { gameObject->GetLogger() }, source->GetPath().c_str(), gameObject->GetResourceManager() },
				gameObject->GetGraphicsDevice()
			};
			auto cloned = std::make_shared<MaterialResource>(*source, descriptor);

			auto sourceData = source->GetData();
			if (!sourceData.empty())
			{
				cloned->SetData(sourceData);
			}

			return cloned;
		}

	private:
		//Player& m_player;
		World& m_world;
		const SceneModeManager& m_modeManager;
	};
}