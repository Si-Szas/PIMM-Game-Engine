#pragma once
#include <PIMM/Core/Core.h>
#include <PIMM/Core/Common.h>
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Game/World.h>
#include <PIMM/ImGui/imgui.h>

#include <../../Game/Player/Player.h>
#include <PIMM/AGameObject/Quad.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/AGameObject/Capsule.h>
#include <PIMM/AGameObject/MeshObject.h>

#include <ranges>

class HierarchyPanel final : public pimm::APanel
{
public:
    HierarchyPanel(pimm::World& world)
        : APanel("Hierarchy"), m_world(world) {
    }

    void Render() override {
        ImGui::Begin("Hierarchy");

            auto gameObjects = m_world.GetAllGameObjects();
            for (auto* object : gameObjects)
            {
                if (!object) continue;
                if (object->GetParent() != nullptr) continue; 
                RenderNode(object);
            }

            // "Unparenting area"
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

            ImGui::End();
        }

    private:
        static const char* GetObjectLabel(pimm::AGameObject* object)
        {
            if (object->GetTypeID() == pimm::Quad::getTypeId()) return "Floor";
            if (object->GetTypeID() == pimm::Cube::getTypeId()) return "Cube";
            if (object->GetTypeID() == pimm::Sphere::getTypeId()) return "Sphere";
            if (object->GetTypeID() == pimm::Cylinder::getTypeId()) return "Cylinder";
            if (object->GetTypeID() == pimm::Capsule::getTypeId()) return "Capsule";
            if (object->GetTypeID() == pimm::Player::getTypeId()) return "Player";
            if (object->GetTypeID() == pimm::MeshObject::getTypeId()) return "Mesh";
            return "GameObject";
        }

        void RenderNode(pimm::AGameObject* object)
        {
            ImGui::PushID(object);

            const bool hasChildren = !object->GetChildren().empty();
            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
            if (!hasChildren) flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet;
            if (object == m_selected) flags |= ImGuiTreeNodeFlags_Selected;

            bool open = ImGui::TreeNodeEx(GetObjectLabel(object), flags);

            if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
            {
                m_selected = object;
            }

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
            {
                ImGui::SetDragDropPayload("HIERARCHY_ITEM", &object, sizeof(pimm::AGameObject*));
                ImGui::Text("%s", GetObjectLabel(object));
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
                        RenderNode(child);
                    }
                }
                ImGui::TreePop();
            }

            ImGui::PopID();
        }

    private:
        pimm::World& m_world;
        pimm::AGameObject* m_selected{ nullptr };
};