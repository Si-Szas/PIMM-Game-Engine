#include <PIMM/InputSystem/InputSystem.h>
#include <PIMM/Core/Identifier.h>
#include <PIMM/UIManager/UIManager.h>
#include <PIMM/InputSystem/Commands/MoveForwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveLeftCommand.h>
#include <PIMM/InputSystem/Commands/MoveBackwardCommand.h>
#include <PIMM/InputSystem/Commands/MoveRightCommand.h>
#include <PIMM/InputSystem/Commands/CreateAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAGameObjectCommand.h>
#include <PIMM/InputSystem/Commands/DeleteAllAGameObjectsCommand.h>
#include <PIMM/InputSystem/Commands/ExitApplicationCommand.h>

#include <ranges>
#include <Windows.h>

pimm::InputSystem::InputSystem(const InputSystemDescriptor& descriptor) :
	Base(descriptor.base),
	m_world(descriptor.world),
	m_uiManager(descriptor.uiManager)
{
	BindWKeyCommand(new MoveForwardCommand(descriptor));
	BindAKeyCommand(new MoveLeftCommand(descriptor));
	BindSKeyCommand(new MoveBackwardCommand(descriptor));
	BindDKeyCommand(new MoveRightCommand(descriptor));

	BindSpaceKeyCommand(new CreateAGameObjectCommand(descriptor));
	BindBackspaceKeyCommand(new DeleteAGameObjectCommand(descriptor));
	BindDeleteKeyCommand(new DeleteAllAGameObjectsCommand(descriptor));
	BindEscapeKeyCommand(new ExitApplicationCommand(descriptor));
}

void pimm::InputSystem::Update()
{
	m_previousKeys = m_currentKeys;
	m_previousMouseButtons = m_currentMouseButtons;

	for (auto i : std::views::iota(0u, static_cast<std::size_t>(KeyCode::Count)))
	{
		const auto vk = GetInternalKeyCode(static_cast<KeyCode>(i));
		m_currentKeys[i] = (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	for (auto i : std::views::iota(0u, static_cast<std::size_t>(MouseInput::Count)))
	{
		const auto vk = GetInternalMouseInput(static_cast<MouseInput>(i));
		m_currentMouseButtons[i] = (GetAsyncKeyState(vk) & 0x8000) != 0;
	}

	m_previousMousePosition = m_mousePosition;

	POINT point{};
	GetCursorPos(&point);

	m_mousePosition.x = static_cast<f32>(point.x);
	m_mousePosition.y = static_cast<f32>(point.y);

	m_mouseDelta.x = m_mousePosition.x - m_previousMousePosition.x;
	m_mouseDelta.y = m_mousePosition.y - m_previousMousePosition.y;

	if (m_cursorLocked) CenterCursor();
}

pimm::InputCommand* pimm::InputSystem::HandleInput()
{
	if (m_uiManager != nullptr &&
		(m_uiManager->WantsCaptureMouse() || m_uiManager->WantsCaptureKeyboard()))
	{
		return NULL;
	}

	if (IsKeyDown(pimm::KeyCode::W)) return WKeyCommand;
	else if (IsKeyDown(pimm::KeyCode::A)) return AKeyCommand;
	else if (IsKeyDown(pimm::KeyCode::S)) return SKeyCommand;
	else if (IsKeyDown(pimm::KeyCode::D)) return DKeyCommand;

	else if (IsKeyPressed(pimm::KeyCode::Space))
	{
		//PIMMLogInformation("Space Key Pressed");
		return spaceKeyCommand;
	}
	else if (IsKeyPressed(pimm::KeyCode::Backspace))
	{
		//PIMMLogInformation("Backspace Key Pressed");
		return backspaceKeyCommand;
	}
	else if (IsKeyPressed(pimm::KeyCode::Delete))
	{
		//PIMMLogInformation("Delete Key Pressed");
		return deleteKeyCommand;
	}
	else if (IsKeyPressed(pimm::KeyCode::Escape))
	{
		//PIMMLogInformation("Escape Key Pressed");
		return escapeKeyCommand;
	}

	else return NULL;
}

void pimm::InputSystem::RecordCommand(InputCommand* command)
{

	m_undoList.push_back(command);
	m_redoList.clear();
}

void pimm::InputSystem::UndoCommand(AGameObject& gameObject)
{
	//If the undo queue is empty, do not allow player to undo
	if (m_undoList.empty()) return;

	//Else, get the end of the undo queue
	InputCommand* command = m_undoList.back();
	m_undoList.pop_back();
	//Undo the command
	command->UndoCommand(gameObject);
	//Push undoed command into redo queue in case user wants to redo
	m_redoList.push_back(command);
}

void pimm::InputSystem::RedoCommand(AGameObject& gameObject)
{
	//Similar implementation to UndoCommand
	if (m_redoList.empty()) return;

	//Get redoed command
	InputCommand* command = m_redoList.back();
	m_redoList.pop_back();
	//Reexecute the command
	command->ExecuteCommand(gameObject, *m_world);
	//Add redoed command to undo list
	m_undoList.push_back(command);
}

void pimm::InputSystem::BindWKeyCommand(InputCommand* newCommandBind)
{
	//If the key is already binded somewhere, delete that
	if (WKeyCommand != nullptr) delete WKeyCommand;

	WKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindAKeyCommand(InputCommand* newCommandBind)
{
	if (AKeyCommand != nullptr) delete AKeyCommand;

	AKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindSKeyCommand(InputCommand* newCommandBind)
{
	if (SKeyCommand != nullptr) delete SKeyCommand;

	SKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindDKeyCommand(InputCommand* newCommandBind)
{
	if (DKeyCommand != nullptr) delete DKeyCommand;

	DKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindSpaceKeyCommand(InputCommand* newCommandBind)
{
	if (spaceKeyCommand != nullptr) delete spaceKeyCommand;

	spaceKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindBackspaceKeyCommand(InputCommand* newCommandBind)
{
	if (backspaceKeyCommand != nullptr) delete backspaceKeyCommand;

	backspaceKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindDeleteKeyCommand(InputCommand* newCommandBind)
{
	if (deleteKeyCommand != nullptr) delete deleteKeyCommand;

	deleteKeyCommand = newCommandBind;
}

void pimm::InputSystem::BindEscapeKeyCommand(InputCommand* newCommandBind)
{
	if (escapeKeyCommand != nullptr) delete escapeKeyCommand;

	escapeKeyCommand = newCommandBind;
}

bool pimm::InputSystem::IsKeyDown(KeyCode key) const
{
	return m_currentKeys[static_cast<std::size_t>(key)];
}

bool pimm::InputSystem::IsKeyPressed(KeyCode key) const
{
	return m_currentKeys[static_cast<std::size_t>(key)] &&
		!m_previousKeys[static_cast<std::size_t>(key)];
}

bool pimm::InputSystem::IsKeyReleased(KeyCode key) const
{
	return !m_currentKeys[static_cast<std::size_t>(key)] &&
		m_previousKeys[static_cast<std::size_t>(key)];
}

bool pimm::InputSystem::IsMouseButtonDown(MouseInput button) const
{
	return m_currentMouseButtons[static_cast<std::size_t>(button)];
}

bool pimm::InputSystem::IsMouseButtonPressed(MouseInput button) const
{
	return m_currentMouseButtons[static_cast<std::size_t>(button)] &&
		!m_previousMouseButtons[static_cast<std::size_t>(button)];
}

bool pimm::InputSystem::IsMouseButtonReleased(MouseInput button) const
{
	return !m_currentMouseButtons[static_cast<std::size_t>(button)] &&
		m_previousMouseButtons[static_cast<std::size_t>(button)];
}

pimm::Vec2 pimm::InputSystem::GetMousePosition() const noexcept
{
	return m_mousePosition;
}

pimm::Vec2 pimm::InputSystem::GetMouseDelta() const noexcept
{
	return m_mouseDelta;
}

void pimm::InputSystem::SetWorld(World& world)
{
	m_world = &world;
}

void pimm::InputSystem::SetUIManager(UIManager& uiManager)
{
	m_uiManager = &uiManager;
}

void pimm::InputSystem::SetCursorVisible(bool visible)
{
	m_cursorVisible = visible;

	while (ShowCursor(visible) < 0 && visible) {}
	while (ShowCursor(visible) >= 0 && !visible) {}
}

void pimm::InputSystem::SetCursorLocked(bool locked)
{
	m_cursorLocked = locked;
	if (locked) CenterCursor();
}

void pimm::InputSystem::SetCursorLockArea(const Rect& lockedArea)
{
	m_lockedArea = lockedArea;
}

short pimm::InputSystem::GetInternalKeyCode(const KeyCode& key)
{
	const auto value = static_cast<int>(key);
	//Key inputs from A to Z
	if (key >= KeyCode::A && key <= KeyCode::Z) return 'A' + (value - static_cast<int>(KeyCode::A));
	//Key inputs from 0 to 9
	if (key >= KeyCode::Num0 && key <= KeyCode::Num9) return '0' + (value - static_cast<int>(KeyCode::Num0));

	switch (key)
	{
	case KeyCode::Shift: return VK_SHIFT;
	case KeyCode::Escape: return VK_ESCAPE;
	case KeyCode::Space: return VK_SPACE;
	case KeyCode::Enter: return VK_RETURN;
	case KeyCode::LeftControl: return VK_LCONTROL;
	case KeyCode::RightControl: return VK_RCONTROL;
	case KeyCode::Up: return VK_UP;
	case KeyCode::Down: return VK_DOWN;
	case KeyCode::Left: return VK_LEFT;
	case KeyCode::Right: return VK_RIGHT;
	case KeyCode::Backspace: return VK_BACK;
	case KeyCode::Delete: return VK_DELETE;
	default: return 0;
	}
}

short pimm::InputSystem::GetInternalMouseInput(const MouseInput& mouseInput)
{
	switch (mouseInput)
	{
	case MouseInput::MouseLeft: return VK_LBUTTON;
	case MouseInput::MouseMiddle: return VK_MBUTTON;
	case MouseInput::MouseRight: return VK_RBUTTON;
	default: return 0;
	}
}

void pimm::InputSystem::CenterCursor()
{
	const auto centerX = m_lockedArea.left + (m_lockedArea.width / 2);
	const auto centerY = m_lockedArea.top + (m_lockedArea.height / 2);

	SetCursorPos(centerX, centerY);

	m_mousePosition.x = static_cast<f32>(centerX);
	m_mousePosition.y = static_cast<f32>(centerY);
}

pimm::InputSystem::~InputSystem()
{
	delete WKeyCommand;
	delete AKeyCommand;
	delete SKeyCommand;
	delete DKeyCommand;
	delete spaceKeyCommand;
	delete backspaceKeyCommand;
	delete deleteKeyCommand;
	delete escapeKeyCommand;
}