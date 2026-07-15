#pragma once
#include <PIMM/Window/Window.h>

namespace pimm
{
	class Display final : public Window
	{
		public:
			//CONSTRUCTOR
			explicit Display(const DisplayDescriptor& descriptor);

			//GETTER
			SwapChain& GetSwapChain() noexcept;
			void* GetHandle() noexcept;

		private:
			RefPtr<SwapChain> m_swapChain{};
	};
}
