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
			// Switch case for color coding log messages based on log level
			switch (m_logger.getDebugLogLevel())
			{
				// Error : Red
			case pimm::Logger::LogLevel::Error: ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), log.c_str()); break;
				// Warning : Yellow
			case pimm::Logger::LogLevel::Warning: ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), log.c_str()); break;
				//Information : White
			case pimm::Logger::LogLevel::Information: ImGui::Text(log.c_str()); break;
			}
		}

		ImGui::End();
	}

private:
	// Uses the Engine Logger to get log messages and print them to this panel
	pimm::Logger& m_logger;
};