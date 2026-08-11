#include <PIMM/AComponent/ControllerComponent.h>
#include <PIMM/Game/World.h>
#include <PIMM/AComponent/TransformComponent.h>
#include <PIMM/InputSystem/InputSystem.h>
#include <PIMM/InputSystem/InputCommand.h>
#include <PIMM/InputSystem/Commands/MoveForwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveRightCommand.h>
#include <PIMM/InputSystem/Commands/MoveLeftCommand.h>
#include <PIMM/InputSystem/Commands/MoveBackwardCommand.h>
#include <PIMM/Math/Vec3.h>

namespace pimm
{
	ControllerComponent::ControllerComponent(const AComponentDescriptor& descriptor)
		: AComponent(descriptor)
	{
	}

	void ControllerComponent::Update(f32 deltaTime)
	{
		if (!m_world.IsPlayMode())
			return;

		auto& inputSystem = m_object.GetInputSystem();

		if (inputSystem.IsMouseButtonDown(MouseInput::MouseRight))
		{
			auto rotation = m_object.GetTransform().GetRotation();

			rotation.x += inputSystem.GetMouseDelta().y * m_sensitivity;
			rotation.y += inputSystem.GetMouseDelta().x * m_sensitivity;

			if (rotation.x > 100.0f)
				rotation.x = 100.0f;
			else if (rotation.x < -100.0f)
				rotation.x = -100.0f;

			m_object.GetTransform().SetRotation(rotation);
		}

		m_object.ResetMovementModifiers();

		InputCommand* command = inputSystem.HandleInput();
		if (command)
		{
			size_t commandType = command->GetTypeID();

			if (commandType == MoveForwardCommand::getTypeId()) command->ExecuteCommand(m_object, m_world);
			if (commandType == MoveRightCommand::getTypeId()) command->ExecuteCommand(m_object, m_world);
			if (commandType == MoveLeftCommand::getTypeId()) command->ExecuteCommand(m_object, m_world);
			if (commandType == MoveBackwardCommand::getTypeId()) command->ExecuteCommand(m_object, m_world);

			inputSystem.RecordCommand(command);
		}

		auto forwardDir = m_object.GetTransform().Forward() * m_object.GetForwardModifier();
		auto rightDir = m_object.GetTransform().Right() * m_object.GetRightModifier();
		auto directionSum = forwardDir + rightDir;

		if (Vec3::LengthSquared(directionSum) > 0.001f)
		{
			auto direction = Vec3::Normalize(directionSum);
			auto position = m_object.GetTransform().GetPosition();
			position = position + direction * m_speedModifier * deltaTime;
			m_object.GetTransform().SetPosition(position);
		}
	}
}
