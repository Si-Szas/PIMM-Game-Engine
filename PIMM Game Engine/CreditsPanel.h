#pragma once
#include <PIMM/UIManager/APanel.h>
#include <PIMM/ImGui/imgui.h>

class CreditsPanel final : public pimm::APanel
{
public:
	CreditsPanel() : APanel("Credits") {}

	void Render() override
	{

		auto filename = "dlsu.png";

		ImGui::Begin("Credits");

		ImGui::Text("About: \n\n");
		ImGui::Text("Scene Editor 0.1x");
		ImGui::Text("Developed by Peter Gabriel C. Abada\n\n");

		ImGui::Text("Acknowledgements: ");
		ImGui::Text("PardCode DirectX 11 Game Engine Tutorial Series");
		ImGui::Text("GDENG03 by Sir Martin");


		ImGui::End();
	}
};