#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Game/World.h>
#include <PIMM/ImGui/imgui.h>
#include "../Player/Player.h"

//COMPONENTS
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/RigidBodyComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AComponent/MaterialComponent.h>
#include <PIMM/AComponent/MeshComponent.h>
#include <PIMM/Resource/MeshResource.h>
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/AGameObject/MeshObject.h>

namespace pimm
{
	class InspectorPanel final : public APanel
	{
	public:
		explicit InspectorPanel(World& world) : APanel("Inspector"), m_world(world) {}

		void Render() override
		{
			ImGui::Begin("Inspector");
			AGameObject* gameObject = m_world.GetSelectedGameObject();

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

							if (ImGui::DragFloat3("Position", positionArr, 0.1f))
								transform.SetPosition({ positionArr[0], positionArr[1], positionArr[2] });

							if (ImGui::DragFloat3("Rotation", rotationArr, 0.1f))
								transform.SetRotation({ rotationArr[0], rotationArr[1], rotationArr[2] });

							if (ImGui::DragFloat3("Scale", scaleArr, 0.1f))
								transform.SetScale({ scaleArr[0], scaleArr[1], scaleArr[2] });
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

										//Display the colors of the material, but also allow the user to control the color if they wanted to
										if (ImGui::ColorEdit3("Material Color", color))
										{
											pimm::Vec3 updatedColor(color[0], color[1], color[2]);
											materialResource->SetData(std::as_bytes(std::span{ &updatedColor, 1 }));
										}
									}
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

												if (ImGui::ColorEdit3("Material Color", color))
												{
													pimm::Vec3 updatedColor(color[0], color[1], color[2]);
													materialResource->SetData(std::as_bytes(std::span{ &updatedColor, 1 }));
												}
											}
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
						}

						//Camera Component
						if (componentId == pimm::RigidBodyComponent::getTypeId())
						{
							ImGui::Separator();
							ImGui::Text("Camera");
							ImGui::NewLine();
						}
					}
				}
			}
			ImGui::End();
		}

	private:
		//Player& m_player;
		World& m_world;
	};
}