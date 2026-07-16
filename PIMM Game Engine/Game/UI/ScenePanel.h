#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>
class ScenePanel final : public pimm::APanel
{
public:
    ScenePanel() : APanel("Scene") {}

    void Render() override
    {
        ImGui::Begin("Scene");

        ImGui::End();
    }
};