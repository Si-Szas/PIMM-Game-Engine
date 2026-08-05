//All Useful Classes and Header files are contained in this header file

#pragma once
//GAME AND WORLD HEADER//
#include <PIMM/Game/Game.h>
#include <PIMM/Game/World.h>
//GAME OBJECTS HEADER//
#include <PIMM/AGameObject/AGameObject.h>
#include <PIMM/AGameObject/Quad.h>
#include <PIMM/AGameObject/Cube.h>
#include <PIMM/AGameObject/Sphere.h>
#include <PIMM/AGameObject/Cylinder.h>
#include <PIMM/AGameObject/Capsule.h>
//COMPONENTS HEADER//
#include <PIMM/AComponent/AComponent.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/AComponent/CameraComponent.h>
#include <PIMM/AComponent/QuadComponent.h>
#include <PIMM/AComponent/CubeComponent.h>
#include <PIMM/AComponent/SphereComponent.h>
#include <PIMM/AComponent/CapsuleComponent.h>
#include <PIMM/AComponent/MeshComponent.h>
//INPUT SYSTEM HEADER//
#include <PIMM/InputSystem/InputSystem.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/InputSystem/Commands/MoveForwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveBackwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveLeftCommand.h>
#include <PIMM/InputSystem/Commands/MoveRightCommand.h>
#include <PIMM/InputSystem/Commands/CreateAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAllAGameObjectsCommand.h>
#include <PIMM/InputSystem/Commands/ExitApplicationCommand.h>
//UI
#include <PIMM/UIManager/UIManager.h>
#include "UI/HierarchyPanel.h"
#include "UI/InspectorPanel.h"
#include "UI/AssetsPanel.h"
#include "UI/ScenePanel.h"
//MATERIALS//
#include <PIMM/Resource/MaterialResource.h>
#include <PIMM/Resource/ResourceManager.h>
//TEXTURES//
#include <PIMM/Resource/TextureResource.h>
#include <PIMM/Graphics/Sampler/Sampler.h>
//MESHES//
#include <PIMM/Resource/MeshResource.h>
