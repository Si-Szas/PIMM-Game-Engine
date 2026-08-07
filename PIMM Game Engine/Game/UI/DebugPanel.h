#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/Core/Logger.h>

#include <PIMM/ImGui/imgui.h>

class DebugPanel final : public pimm::APanel
{
public:
	DebugPanel(pimm::Logger& logger) : 
		APanel("Debug"), m_logger(logger) {}

	void Render() override
	{
		ImGui::Begin("Debug");

		

		ImGui::End();
	}

private:
	pimm::Logger& m_logger;
};