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

		//Foreach loop to print log messages from the logger to the panel
		for (const auto& log : m_logger.getLogMessages())
		{
			ImGui::Text(log.c_str());
		}

		ImGui::End();
	}

private:
	// Uses the Engine Logger to get log messages and print them to this panel
	pimm::Logger& m_logger;
};