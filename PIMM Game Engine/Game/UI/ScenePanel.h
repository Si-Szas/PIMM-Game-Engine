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

        pimm::ui32 vpCount = m_worldRenderer.GetViewportCount();
        if (vpCount == 1)
        {
            m_worldRenderer.SetSceneViewSize(pimm::Rect{ static_cast<pimm::i32>(avail.x), static_cast<pimm::i32>(avail.y) });
            auto* frameBuffer = m_worldRenderer.GetFrameBuffer();
            if (frameBuffer)
                ImGui::Image(reinterpret_cast<ImTextureID>(frameBuffer->GetSRV()), avail);
        }
        else
        {
            pimm::f32 halfW = avail.x * 0.5f;
            pimm::f32 halfH = avail.y * 0.5f;
            m_worldRenderer.SetSceneViewSize(pimm::Rect{ static_cast<pimm::i32>(halfW), static_cast<pimm::i32>(halfH) });

            for (pimm::ui32 i = 0; i < vpCount; ++i)
            {
                auto* fb = m_worldRenderer.GetViewportFrameBuffer(i);
                if (fb)
                {
                    ImGui::Image(reinterpret_cast<ImTextureID>(fb->GetSRV()), { halfW, halfH });
                    if ((i % 2) == 0)
                        ImGui::SameLine();
                }
            }
        }

        ImGui::End();
    }
private:
    pimm::WorldRenderer& m_worldRenderer;
};