#pragma once
#include <PIMM/Graphics/UIManager/APanel.h>
#include <PIMM/Graphics/ImGui/imgui.h>

class ScenePanel final : public pimm::APanel
{
public:
    ScenePanel() : APanel("Scene") {}

    void Render() override
    {
        ImGui::Begin("Scenes");


        ImGui::End();
    }
};