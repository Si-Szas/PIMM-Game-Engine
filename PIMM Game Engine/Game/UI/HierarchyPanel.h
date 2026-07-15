#pragma once
#include <PIMM/Graphics/UIManager/APanel.h>
#include <PIMM/Graphics/ImGui/imgui.h>

class HierarchyPanel : public pimm::APanel {
public:
    void Render() override {
        ImGui::Begin("Hierarchy");
        ImGui::Selectable("Player");
        ImGui::Selectable("Floor");
        ImGui::Selectable("Cube");
        ImGui::Selectable("Sphere");
        ImGui::Selectable("Cylinder");
        ImGui::Selectable("Capsule");
        ImGui::End();
    }
};
