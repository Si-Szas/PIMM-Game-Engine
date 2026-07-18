#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>
#include "../Player/Player.h"
#include <PIMM/AComponent/TransformComponent.h>
namespace pimm
{
	class InspectorPanel final : public APanel
	{
	public:
		explicit InspectorPanel(Player& player) : APanel("Inspector"), m_player(player) {}
		void Render() override
		{
			ImGui::Begin("Inspector");
			ImGui::Text("Player");
			ImGui::NewLine();
			ImGui::Text("Transform");

			auto& transform = m_player.GetTransform();

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

			ImGui::End();
		}

	private:
		Player& m_player;
	};
}