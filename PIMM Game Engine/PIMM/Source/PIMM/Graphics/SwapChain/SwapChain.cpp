#include <PIMM/Graphics/SwapChain/SwapChain.h>

pimm::SwapChain::SwapChain(const SwapChainDescriptor& swapChainDescriptor, const GraphicsResourceDescriptor& graphicsResourceDescriptor):
	GraphicsResource(graphicsResourceDescriptor), m_size(swapChainDescriptor.windowSize)
{
	if (!swapChainDescriptor.windowHandle) PIMMLogThrowInvalidArgument("No Window Handle provided.");

	DXGI_SWAP_CHAIN_DESC dxgiDescriptor{};

	//Define attributes of buffers used by swap chain
	//Canvas where graphics will be rendered
	dxgiDescriptor.BufferDesc.Width = std::max(1, swapChainDescriptor.windowSize.width);
	dxgiDescriptor.BufferDesc.Height = std::max(1, swapChainDescriptor.windowSize.height);
	//UNORM means normalized, unsigned integers
	dxgiDescriptor.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//How many backbuffers
	//If swap chain is in windowed mode, only 2 back buffers (front not included), if fullscreen, then has 3 back buffers
	dxgiDescriptor.BufferCount = 2;
	//Tells DXGI that we'll use the buffer as our render target
	dxgiDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	dxgiDescriptor.OutputWindow = static_cast<HWND>(swapChainDescriptor.windowHandle);
	//Set sample counter for multi sample anti-aliasing
	dxgiDescriptor.SampleDesc.Count = 1;
	//Back buffer is not copied to a separate surface
	//Back buffers created in swap chain is shared with the DWM
	dxgiDescriptor.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	dxgiDescriptor.Windowed = TRUE;

	//Fails if we pass 0 negative values for the width or the height, which is why we use the standard max function
	PIMMGraphicsLogThrowOnFail(m_dxgiFactory.CreateSwapChain
	(
		&m_d3dDevice,		//Pass device, type IUknown, since it can accept multiple types of devices
		&dxgiDescriptor,	//Address of swap chain descriptor
		&m_swapChain //Retrieve output parameter, dxgi swapp chain
	),
		"CreateSwapChain() failed.");

	//Ensures that RTV of back buffer ready to be used by Device Context
	ReloadBuffers();
}

void pimm::SwapChain::Present(bool vsync)
{
	auto hr =
		m_swapChain->Present
		(
			vsync,		//Synchronization interval (syncrhonizes frame presentation with monitor's vertical refresh rate
			0			//Presentation flag
		);

	if (FAILED(hr))
	{
		PIMMLogError("Present() failed.");
		return;
	}
}

pimm::Rect pimm::SwapChain::GetSize() const noexcept
{
	return m_size;
}

ID3D11RenderTargetView* pimm::SwapChain::GetRenderTargetView() const noexcept
{
	return m_renderTargetView.Get();
}

ID3D11DepthStencilView* pimm::SwapChain::GetDepthStencilView() const noexcept
{
	return m_depthStencilView.Get();
}

void pimm::SwapChain::ReloadBuffers()
{
	//Retrieve the back buffer and 
	// Create the render target view from it

	Microsoft::WRL::ComPtr<ID3D11Texture2D> buffer{};

	//Retrieve back buffer
	PIMMGraphicsLogThrowOnFail(
	m_swapChain->GetBuffer(
		0,						//Buffer index, always use 0
		IID_PPV_ARGS(&buffer)	//Pass pointer where buffer will be stored
	), "GetBuffer() failed.");

	//Create Render Target View from D3D Device
	PIMMGraphicsLogThrowOnFail(
	m_d3dDevice.CreateRenderTargetView(
		buffer.Get(),			//Texture resource we want to create the view from (we pass the back buffer)
		nullptr,				//Render target view descriptor (nullptr for default)
		&m_renderTargetView		//Pointer to receive RTV instance
		
	), "CreateRenderTargetView() failed.");

	//DEPTH STENCIL//
	D3D11_TEXTURE2D_DESC depthTextureDescriptor = {};
	depthTextureDescriptor.Width = std::max(1, m_size.width);
	depthTextureDescriptor.Height = std::max(1, m_size.height);
	depthTextureDescriptor.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthTextureDescriptor.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthTextureDescriptor.MipLevels = 1;
	depthTextureDescriptor.SampleDesc.Count = 1;
	depthTextureDescriptor.ArraySize = 1;

	PIMMGraphicsLogThrowOnFail(
	m_d3dDevice.CreateTexture2D(
		&depthTextureDescriptor,
		nullptr,
		&buffer
	), "CreateTexture2D() failed.");

	PIMMGraphicsLogThrowOnFail(
	m_d3dDevice.CreateDepthStencilView(
		buffer.Get(),
		NULL,
		&m_depthStencilView
	), "CreateTexture2D() failed.");
}
