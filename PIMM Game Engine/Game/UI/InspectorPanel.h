#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Game/World.h>
#include <PIMM/ImGui/imgui.h>
#include "../Player/Player.h"
#include <PIMM/AComponent/TransformComponent.h>
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
				ImGui::Text(gameObject->GetObjectLabel(gameObject));
				ImGui::NewLine();

				//COMPONENTS

				ImGui::Text("Transform");

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

				ImGui::Separator();
				ImGui::Text("Material");
				static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
				ImGui::ColorEdit4("Color", color);
			}
			ImGui::End();
		}

	private:
		//Player& m_player;
		World& m_world;
	};
}