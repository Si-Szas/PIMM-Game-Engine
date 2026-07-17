#pragma once
#include <PIMM/Graphics/UIManager/APanel.h>
#include <PIMM/Graphics/ImGui/imgui.h>

class ColorPickerPanel : public pimm::APanel
{
public:
    void SetShow(bool show) { m_show = show; }

    void Render() override
    {
        if (!m_show)
            return;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(540, 0), ImGuiCond_Appearing);

        ImGui::Begin("Color Picker", &m_show,
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoCollapse);

        static float color[4] = { 1.0f, 1.0f, 1.0f, 1.0f };

        ImGui::ColorPicker4("Color", color,
            ImGuiColorEditFlags_NoInputs |
            ImGuiColorEditFlags_NoLabel |
            ImGuiColorEditFlags_NoAlpha |
            ImGuiColorEditFlags_DisplayHSV |
            ImGuiColorEditFlags_PickerHueWheel);

        ImGui::Spacing();
        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            m_show = false;
        }

        ImGui::End();
    }

private:
    bool m_show = false;
};
