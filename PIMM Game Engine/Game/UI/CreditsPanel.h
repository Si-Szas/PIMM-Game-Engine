#pragma once
#include <PIMM/Graphics/UIManager/APanel.h>
#include <PIMM/Graphics/ImGui/imgui.h>

class CreditsPanel : public pimm::APanel
{
public:
    void SetShow(bool show) { m_show = show; }
    void SetLogoTexture(ImTextureID texture) { m_logoTexture = texture; }

    void Render() override
    {
        if (!m_show)
            return;

        const ImGuiViewport* viewport = ImGui::GetMainViewport();
        ImVec2 center = viewport->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGui::SetNextWindowSize(ImVec2(400, 0), ImGuiCond_Appearing);

        ImGui::Begin("Credits", &m_show,
            ImGuiWindowFlags_NoDocking |
            ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoCollapse);

        if (m_logoTexture)
        {
            ImGui::Image(m_logoTexture, ImVec2(128, 128));
            ImGui::Spacing();
        }

        ImGui::TextWrapped("PIMM Game Engine");
        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Developed by:");
        ImGui::BulletText("Ivan James M. Peñas");
        ImGui::Spacing();

        ImGui::Separator();
        ImGui::Text("Acknowledgments");
        ImGui::BulletText("PardCode's Tutorials");
        ImGui::BulletText("IMGUI");
        ImGui::BulletText("Sir Martin");
        ImGui::BulletText("Made by the PIMM Team");
        ImGui::Spacing();

        if (ImGui::Button("Close", ImVec2(120, 0)))
        {
            m_show = false;
        }

        ImGui::End();
    }

private:
    bool m_show = false;
    ImTextureID m_logoTexture = 0;
};
