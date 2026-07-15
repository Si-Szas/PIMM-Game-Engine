#include <PIMM/Graphics/UIManager/UIManager.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>
#include <PIMM/Graphics/ImGui/imgui_impl_dx11.h>
#include <PIMM/Graphics/ImGui/imgui_impl_win32.h>
#include <Windows.h>

pimm::UIManager::UIManager(const UIManagerDescriptor& descriptor) :
	Base(descriptor.base),
	m_graphicsDevice(descriptor.graphicsDevice)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	ImGui_ImplWin32_Init(static_cast<HWND>(descriptor.windowHandle));
	ImGui_ImplDX11_Init(
		m_graphicsDevice.GetD3DDevice().Get(),
		m_graphicsDevice.GetD3DDeviceContext().Get()
	);
	ImGui::StyleColorsDark();
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
}

void pimm::UIManager::NewFrame()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void pimm::UIManager::Render()
{
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

bool pimm::UIManager::WantsCaptureMouse() const noexcept
{
	return ImGui::GetIO().WantCaptureMouse;
}

bool pimm::UIManager::WantsCaptureKeyboard() const noexcept
{
	return ImGui::GetIO().WantCaptureKeyboard;
}

pimm::UIManager::~UIManager()
{
	if (ImGui::GetCurrentContext() != nullptr)
	{
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();
	}
}

void pimm::UIManager::RegisterPanel(std::unique_ptr<APanel> panel) {
	m_panels.push_back(std::move(panel));
}

void pimm::UIManager::RenderPanels()
{
	for (size_t i = 0; i < m_panels.size(); ++i)
	{
		m_panels[i]->Render();
	}
}