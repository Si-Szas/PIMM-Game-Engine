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

#include <fstream>
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
        pimm::ui32 totalGameObjects = static_cast<pimm::ui32>(gameObjects.size());

        for (auto i : std::views::iota(0u, totalGameObjects))
        {
            auto object = gameObjects[i];
            if (!object) continue;

            if (object->GetTypeID() == pimm::Quad::getTypeId()) ImGui::Selectable("Floor");
            if (object->GetTypeID() == pimm::Cube::getTypeId()) ImGui::Selectable("Cube");
            if (object->GetTypeID() == pimm::Sphere::getTypeId()) ImGui::Selectable("Sphere");
            if (object->GetTypeID() == pimm::Cylinder::getTypeId()) ImGui::Selectable("Cylinder");
            if (object->GetTypeID() == pimm::Capsule::getTypeId()) ImGui::Selectable("Capsule");
            if (object->GetTypeID() == pimm::Player::getTypeId()) ImGui::Selectable("Player");
        }

        ImGui::End();
    }

    private:
        pimm::World& m_world;
};
