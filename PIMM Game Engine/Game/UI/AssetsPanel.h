#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>

class AssetsPanel : public pimm::APanel
{
public:
    void Render() override
    {
        ImGui::Begin("Assets");

        if (ImGui::TreeNodeEx("Assets", ImGuiTreeNodeFlags_DefaultOpen))
        {
            if (ImGui::TreeNode("Folder 1"))
                ImGui::TreePop();

            if (ImGui::TreeNode("Folder 2"))
                ImGui::TreePop();

            if (ImGui::TreeNode("Folder 3"))
                ImGui::TreePop();

            ImGui::TreePop(); 
        }

        ImGui::End();
    }
};