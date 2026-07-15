#pragma once
#include <PIMM/Graphics/UIManager/APanel.h>
#include <PIMM/Graphics/ImGui/imgui.h>

class InspectorPanel : public pimm::APanel {
public:
    void Render() override {
        ImGui::Begin("Inspector");
        ImGui::Text("Transform");
        static float position[3] = { 0.0f, 0.0f, 0.0f };
        static float rotation[3] = { 0.0f, 0.0f, 0.0f };
        static float scale[3] = { 1.0f, 1.0f, 1.0f };
        ImGui::DragFloat3("Position", position, 0.1f);
        ImGui::DragFloat3("Rotation", rotation, 0.1f);
        ImGui::DragFloat3("Scale", scale, 0.1f);
        ImGui::Separator();
        ImGui::Text("Material");
        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
        ImGui::ColorEdit4("Color", color);
        ImGui::End();
    }
};
