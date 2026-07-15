#include <PIMM/Game/Display.h>
#include <PIMM/Graphics/GraphicsDevice/GraphicsDevice.h>

pimm::Display::Display(const DisplayDescriptor& descriptor): Window(descriptor.window)
{
	m_swapChain = descriptor.graphicsDevice.CreateSwapChain({m_handle, m_size});
}

pimm::SwapChain& pimm::Display::GetSwapChain() noexcept
{
	return *m_swapChain;
}

void* pimm::Display::GetHandle() noexcept
{
	return m_handle;
}
