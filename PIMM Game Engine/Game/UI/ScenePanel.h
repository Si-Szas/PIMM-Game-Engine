#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>
#include <PIMM/Graphics/FrameBuffer/FrameBuffer.h>
#include <PIMM/Game/WorldRenderer.h>
class ScenePanel final : public pimm::APanel
{
public:
    ScenePanel(pimm::WorldRenderer& worldRenderer) :
        APanel("Scene"), m_worldRenderer(worldRenderer) {
    }
    void Render() override
    {
        ImGui::Begin("Scene");
        ImVec2 avail = ImGui::GetContentRegionAvail();
        m_worldRenderer.SetSceneViewSize(pimm::Rect{ static_cast<pimm::i32>(avail.x), static_cast<pimm::i32>(avail.y) });
        auto* frameBuffer = m_worldRenderer.GetFrameBuffer();
        if (frameBuffer)
        {
            ImGui::Image(reinterpret_cast<ImTextureID>(frameBuffer->GetSRV()), avail);
        }
        ImGui::End();
    }
private:
    pimm::WorldRenderer& m_worldRenderer;
};