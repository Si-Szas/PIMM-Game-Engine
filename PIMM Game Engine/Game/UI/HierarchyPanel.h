#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Game/World.h>
#include <PIMM/ImGui/imgui.h>
#include "../Editor/SceneModeManager.h"

#include <../../Game/Player/Player.h>
#include <PIMM/AGameObject/Quad.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/AGameObject/Capsule.h>
#include <PIMM/AGameObject/MeshObject.h>

#include <ranges>
#include <span>

class HierarchyPanel final : public pimm::APanel
{
public:
    HierarchyPanel(pimm::World& world, const pimm::SceneModeManager& modeManager)
        : APanel("Hierarchy"), m_world(world), m_modeManager(modeManager) {
    }

    void Render() override 
    {
        ImGui::Begin("Hierarchy");

            auto gameObjects = m_world.GetAllGameObjects();
            bool isEditMode = m_modeManager.IsEditMode();

            for (auto* object : gameObjects)
            {
                if (!object) continue;

                std::string currentName = object->GetObjectName();
                std::string baseName = CleanName(currentName);

                int duplicateCounter = 0;
                for (auto* otherObject : gameObjects)
                {
                    if (otherObject == object) break;

                    if (otherObject != nullptr)
                    {
                        std::string otherBase = CleanName(otherObject->GetObjectName());
                        if (otherBase == baseName)
                        {
                            duplicateCounter++;
                        }
                    }
                }

                std::string uniqueName = baseName;
                if (duplicateCounter > 0)
                {
                    uniqueName += " (" + std::to_string(duplicateCounter) + ")";
                }

                if (currentName != uniqueName)
                {
                    object->SetObjectName(uniqueName);
                }
            }

            for (pimm::ui32 i = 0; i < gameObjects.size(); i++)
            {
                auto* object = gameObjects[i];

                    if (!object) continue;
                    if (object->GetParent() != nullptr) continue;
                    RenderNode(object, gameObjects);
                }

            if (isEditMode)
            {
                ImGui::InvisibleButton("##HierarchyDropRoot", ImGui::GetContentRegionAvail());
                if (ImGui::BeginDragDropTarget())
                {
                    if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM"))
                    {
                        auto* dropped = *static_cast<pimm::AGameObject* const*>(payload->Data);
                        dropped->SetParent(nullptr);
                    }
                    ImGui::EndDragDropTarget();
                }
            }

            ImGui::End();
    }

private:
    void RenderNode(pimm::AGameObject* object, std::span<pimm::AGameObject* const> gameObjects)
    {
        ImGui::PushID(object);

        const bool hasChildren = !object->GetChildren().empty();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        if (object == m_selected) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(object->GetObjectLabel(object), flags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            m_selected = object;

            auto it = std::ranges::find(gameObjects, object);
            if (it != gameObjects.end())
            {
                pimm::ui32 realIndex = static_cast<pimm::ui32>(std::distance(gameObjects.begin(), it));
                m_world.SetSelectedObjectIndex(realIndex);
            }
        }

        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload("HIERARCHY_ITEM", &object, sizeof(pimm::AGameObject*));
            ImGui::Text("%s", object->GetObjectLabel(object));
            ImGui::EndDragDropSource();
        }

        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("HIERARCHY_ITEM"))
            {
                auto* dropped = *static_cast<pimm::AGameObject* const*>(payload->Data);
                if (dropped != object)
                {
                    dropped->SetParent(object);
                }
            }
            ImGui::EndDragDropTarget();
        }

        if (ImGui::BeginPopupContextItem())
        {
            if (object->GetParent() != nullptr)
            {
                if (ImGui::MenuItem("Unparent"))
                {
                    object->SetParent(nullptr);
                }
            }
            ImGui::EndPopup();
        }

        if (open)
        {
            if (hasChildren)
            {
                for (auto* child : object->GetChildren())
                {
                    RenderNode(child, gameObjects);
                }
            }
            ImGui::TreePop();
        }

        ImGui::PopID();
    }

    std::string CleanName(const std::string& name)
    {
        //Cleans up the name in case of duplicate brackest
        size_t bracket = name.find(" (");
        if (bracket != std::string::npos)
        {
            return name.substr(0, bracket);
        }
        return name;
    }

    private:
        pimm::World& m_world;
        const pimm::SceneModeManager& m_modeManager;
        pimm::AGameObject* m_selected{ nullptr };
};