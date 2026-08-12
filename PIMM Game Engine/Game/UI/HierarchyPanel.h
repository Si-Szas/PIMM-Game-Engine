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

            {
                float searchWidth = ImGui::GetContentRegionAvail().x;
                ImGui::SetNextItemWidth(searchWidth);
                static char searchBuf[256] = {};
                strcpy_s(searchBuf, m_world.GetSearchFilter().c_str());
                if (ImGui::InputTextWithHint("##HierarchySearch", "Search...", searchBuf, sizeof(searchBuf)))
                    m_world.SetSearchFilter(searchBuf);
                if (!m_world.GetSearchFilter().empty())
                {
                    ImGui::SameLine();
                    if (ImGui::SmallButton("X"))
                        m_world.SetSearchFilter("");
                }
            }

            ImGui::Separator();

            auto gameObjects = m_world.GetAllGameObjects();
            bool isEditMode = m_modeManager.IsEditMode();
            const auto& searchFilter = m_world.GetSearchFilter();
            bool hasFilter = !searchFilter.empty();

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
                    if (object->GetTypeID() == pimm::Player::getTypeId()) continue;
                    if (object->GetParent() != nullptr) continue;
                    if (hasFilter && !ObjectMatchesFilter(object, searchFilter))
                        continue;
                    RenderNode(object, gameObjects);
                }

            if (isEditMode && !hasFilter)
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

            if (isEditMode && ImGui::IsKeyPressed(ImGuiKey_Delete) && ImGui::IsWindowFocused())
            {
                auto selected = m_world.GetSelectedGameObjects();
                for (auto* obj : selected)
                {
                    if (obj && obj->GetTypeID() != pimm::Player::getTypeId())
                        m_world.DestroyAGameObjectInternal(obj);
                }
                m_world.DeselectAllObjects();
            }

            ImGui::End();
    }

private:
    bool ObjectMatchesFilter(pimm::AGameObject* object, const std::string& filter)
    {
        if (filter.empty())
            return true;
        std::string name = object->GetObjectName();
        std::string nameLower;
        std::string filterLower;
        nameLower.resize(name.size());
        filterLower.resize(filter.size());
        std::transform(name.begin(), name.end(), nameLower.begin(), ::tolower);
        std::transform(filter.begin(), filter.end(), filterLower.begin(), ::tolower);
        return nameLower.find(filterLower) != std::string::npos;
    }

    void RenderNode(pimm::AGameObject* object, std::span<pimm::AGameObject* const> gameObjects)
    {
        ImGui::PushID(object);

        const bool hasChildren = !object->GetChildren().empty();
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
        if (m_world.IsSelected(object)) flags |= ImGuiTreeNodeFlags_Selected;

        bool open = ImGui::TreeNodeEx(object->GetObjectLabel(object), flags);

        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
        {
            bool ctrlHeld = ImGui::GetIO().KeyCtrl;
            bool shiftHeld = ImGui::GetIO().KeyShift;

            if (ctrlHeld)
            {
                m_world.ToggleSelection(object);
                m_lastClicked = object;
            }
            else if (shiftHeld && m_lastClicked)
            {
                HandleRangeSelect(gameObjects, m_lastClicked, object);
            }
            else
            {
                m_world.DeselectAllObjects();
                m_world.SelectObject(object);
                m_lastClicked = object;
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

    void HandleRangeSelect(std::span<pimm::AGameObject* const> gameObjects, pimm::AGameObject* from, pimm::AGameObject* to)
    {
        auto itFrom = std::find(gameObjects.begin(), gameObjects.end(), from);
        auto itTo = std::find(gameObjects.begin(), gameObjects.end(), to);
        if (itFrom == gameObjects.end() || itTo == gameObjects.end())
            return;

        if (itFrom > itTo)
            std::swap(itFrom, itTo);

        for (auto it = itFrom; it <= itTo; ++it)
        {
            if (*it && (*it)->GetTypeID() != pimm::Player::getTypeId())
                m_world.SelectObject(*it);
        }
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
        pimm::AGameObject* m_lastClicked{ nullptr };
};