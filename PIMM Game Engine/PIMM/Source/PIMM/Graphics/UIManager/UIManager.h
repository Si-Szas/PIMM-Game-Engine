#pragma once
#include <PIMM/Core/Base.h>
#include <PIMM/Core/Core.h>
#include <vector>
#include <memory>
#include <PIMM/Graphics/UIManager/APanel.h>

namespace pimm
{
	class UIManager final : public Base
	{
	public:
		explicit UIManager(const UIManagerDescriptor& descriptor);

		void NewFrame();
		void Render();

		bool WantsCaptureMouse() const noexcept;
		bool WantsCaptureKeyboard() const noexcept;

		virtual ~UIManager() override;

	public:
		void RegisterPanel(std::unique_ptr<APanel> panel);

		void RenderPanels();

	private:
		std::vector<std::unique_ptr<APanel>> m_panels;
		GraphicsDevice& m_graphicsDevice;
	};
}