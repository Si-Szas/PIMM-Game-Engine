#include "Player.h"
#include <PIMM/Game/WorldRenderer.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/InputSystem/InputSystem.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/InputSystem/Commands/MoveForwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveRightCommand.h>
#include <PIMM/InputSystem/Commands/MoveLeftCommand.h>
#include <PIMM/InputSystem/Commands/MoveBackwardCommand.h>
#include <PIMM/InputSystem/Commands/CreateAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAllAGameObjectsCommand.h>
#include <PIMM/InputSystem/Commands/ExitApplicationCommand.h>

#include <iostream>

pimm::Player::Player(const AGameObjectDescriptor& descriptor) :
	AGameObject(descriptor)
{
}

void pimm::Player::OnCreate()
{
	m_camera = CreateOrGetComponent<CameraComponent>();
}

void pimm::Player::OnUpdate(f32 deltaTime)
{
	auto& world = GetWorld();
	auto& inputSystem = GetInputSystem();
	auto& resourceManager = GetResourceManager();

	auto sensitivity = 0.1f;

	if (inputSystem.IsMouseButtonDown(MouseInput::MouseRight))
	{
		auto rotation = GetTransform().GetRotation();

		rotation.x += inputSystem.GetMouseDelta().y * sensitivity;
		rotation.y += inputSystem.GetMouseDelta().x * sensitivity;

		if (rotation.x > 100.0f)
			rotation.x = 100.0f;
		else if (rotation.x < -100.0f)
			rotation.x = -100.0f;

		GetTransform().SetRotation(rotation);
	}
	if (inputSystem.IsMouseButtonPressed(MouseInput::MouseRight))
	{
		inputSystem.SetCursorVisible(false);
		inputSystem.SetCursorLocked(true);
	}

	if (inputSystem.IsMouseButtonReleased(MouseInput::MouseRight))
	{
		inputSystem.SetCursorVisible(true);
		inputSystem.SetCursorLocked(false);
	}
	auto position = GetTransform().GetPosition();

	ResetMovementModifiers();

	InputCommand* command = inputSystem.HandleInput();
	if (command)
	{
		size_t commandType = command->GetTypeID();

		if (commandType == pimm::MoveForwardCommand::getTypeId()) command->ExecuteCommand(*this, world);
		if (commandType == pimm::MoveRightCommand::getTypeId()) command->ExecuteCommand(*this, world);
		if (commandType == pimm::MoveLeftCommand::getTypeId()) command->ExecuteCommand(*this, world);
		if (commandType == pimm::MoveBackwardCommand::getTypeId()) command->ExecuteCommand(*this, world);
		if (commandType == pimm::CreateAGameObjectCommand::getTypeId()) command->ExecuteCommand(*this, world, resourceManager);
		if (commandType == pimm::DeleteAGameObjectCommand::getTypeId()) command->ExecuteCommand(*this, world);
		//if (commandType == pimm::DeleteAllAGameObjectsCommand::getTypeId()) command->ExecuteCommand(*this, world);
		if (commandType == pimm::ExitApplicationCommand::getTypeId()) command->ExecuteCommand();

		//Record the command that was just executed
		inputSystem.RecordCommand(command);
	}

	/// TEMPORARY ///
	//if (inputSystem.IsKeyDown(pimm::KeyCode::LeftControl) || inputSystem.IsKeyDown(pimm::KeyCode::RightControl))
	//{
	//	std::cout << "Holding CTRL" << std::endl;
	//	if (inputSystem.IsKeyPressed(pimm::KeyCode::Z)) // Pressed this frame
	//	{
	//		std::cout << "Command Undoed" << std::endl;
	//		inputSystem.UndoCommand(*this);
	//	}
	//	else if (inputSystem.IsKeyPressed(pimm::KeyCode::Y))
	//	{
	//		std::cout << "Command Redoed" << std::endl;
	//		inputSystem.RedoCommand(*this);
	//	}
	//}

	auto forwardDir = GetTransform().Forward() * GetForwardModifier();
	auto rightDir = GetTransform().Right() * GetRightModifier();
	auto directionSum = forwardDir + rightDir;

	if (pimm::Vec3::LengthSquared(directionSum) > 0.001f)
	{
		auto direction = pimm::Vec3::Normalize(directionSum);
		position = position + direction * GetSpeedModifier() * deltaTime;
	}

	GetTransform().SetPosition(position);
}

pimm::Player::~Player()
{
}