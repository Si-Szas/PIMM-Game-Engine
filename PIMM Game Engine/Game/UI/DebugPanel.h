#pragma once
#include <PIMM/UIManager/APanel.h>

#include <PIMM/ImGui/imgui.h>

class DebugPanel final : public pimm::APanel
{
public:
	DebugPanel() : APanel("Debug") {}

	void Render() override
	{
		ImGui::Begin("Debug");

		

		ImGui::End();
	}
};